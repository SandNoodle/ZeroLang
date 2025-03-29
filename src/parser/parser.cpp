#include "parser/parser.h"

#include "ast/ast_operator.h"
#include "ast/nodes/assign.h"
#include "ast/nodes/binary.h"
#include "ast/nodes/for_loop.h"
#include "ast/nodes/foreach_loop.h"
#include "ast/nodes/function_declaration.h"
#include "ast/nodes/if.h"
#include "ast/nodes/literal.h"
#include "ast/nodes/module.h"
#include "ast/nodes/struct_declaration.h"
#include "ast/nodes/unary.h"
#include "ast/nodes/variable_declaration.h"

#include <utility>

namespace soul::parser
{
	using namespace soul::ast;
	using namespace soul::ast::nodes;

	enum class Parser::Precedence : u8
	{
		None,
		Assign,          // =
		Or,              // ||
		And,             // &&
		Equal,           // == !=
		Compare,         // < > <= =>
		Additive,        // + -
		Multiplicative,  // * /
		Unary,           // ! -
		Call,
		Primary,
	};

	struct Parser::PrecedenceRule
	{
		public:
		typedef ast::ASTNode::Dependency (Parser::*PrefixFn)(Parser::Context&);
		typedef ast::ASTNode::Dependency (Parser::*InfixFn)(Parser::Context&, ASTNode::Dependency);
		typedef ast::ASTNode::Dependency (Parser::*SuffixFn)(Parser::Context&);

		public:
		Precedence precedence = Precedence::None;
		PrefixFn   prefix     = nullptr;
		InfixFn    infix      = nullptr;
		SuffixFn   suffix     = nullptr;
	};

	static constexpr auto k_literal_types
		= { TokenType::LiteralInteger,    TokenType::LiteralFloat, TokenType::LiteralString,
		    TokenType::LiteralIdentifier, TokenType::KeywordTrue,  TokenType::KeywordFalse };

	static constexpr auto k_compare_types
		= { TokenType::Less, TokenType::LessEqual, TokenType::Greater, TokenType::GreaterEqual };

	ASTNode::Dependency Parser::parse(std::span<const Token> tokens)
	{
		if (tokens.empty()) {
			return nullptr;
		}

		Context               parsing_context(tokens);
		ASTNode::Dependencies statements{};
		while (!parsing_context.try_match(TokenType::EndOfFile)) {
			auto statement = parse_statement(parsing_context);
			if (statement) {
				statements.push_back(std::move(statement));
			}

			if (parsing_context.had_panic) {
				parsing_context.synchronize();
			}
		}

		return ModuleNode::create("", std::move(statements));
	}

	ASTNode::Dependency Parser::parse_statement(Context& context)
	{
		switch (context.current_token()->type()) {
			case TokenType::KeywordFn:
				return parse_function_declaration(context);
			case TokenType::KeywordFor:
				return parse_for_loop(context);
			case TokenType::KeywordForeach:
				return parse_foreach_loop(context);
			case TokenType::KeywordIf:
				return parse_if(context);
			case TokenType::KeywordLet:
				return parse_variable_declaration(context);
			case TokenType::KeywordStruct:
				return parse_struct_declaration(context);
			case TokenType::KeywordWhile:
				return parse_while_loop(context);
			default:
				break;
		}

		return parse_expression_statement(context);
	}

	ASTNode::Dependency Parser::parse_expression(Context& context)
	{
		// NOTE: Starting precedence has to be at least one higher than no precedence.
		return parse_expression_with_precedence(
			context, static_cast<Parser::Precedence>(std::to_underlying(Parser::Precedence::None) + 1));
	}

	ASTNode::Dependency Parser::parse_expression_statement(Context& context)
	{
		static constexpr auto k_assign_types = {
			TokenType::Equal, TokenType::PlusEqual, TokenType::MinusEqual, TokenType::StarEqual, TokenType::SlashEqual
		};
		if (std::ranges::find(k_assign_types, context.current_token()->type()) != std::end(k_assign_types)) {
			return parse_assign(context);
		}

		return parse_expression(context);
	}

	ASTNode::Dependency Parser::parse_expression_with_precedence(Context& context, Precedence precedence)
	{
		auto prefix_rule = get_precedence_rule(context.current_token()->type()).prefix;
		if (!prefix_rule) {
			// Error!
			return nullptr;
		}
		auto prefix_expression = (this->*prefix_rule)(context);

		TokenType current_type = TokenType::Unknown;
		while (precedence <= get_precedence_rule(context.current_token()->type()).precedence) {
			current_type    = context.current_token()->type();
			auto infix_rule = get_precedence_rule(current_type).infix;
			if (!infix_rule) {
				// Error! Expected expression.
				return nullptr;
			}
			prefix_expression = (this->*infix_rule)(context, std::move(prefix_expression));
		}

		return prefix_expression;
	}

	ASTNode::Dependency Parser::parse_assign(Context& context)
	{
		// <assign_statement> ::= <expression> '=' <expression> ';'

		// <expression>
		auto lhs = parse_expression(context);
		if (!lhs) {
			context.had_panic = true;
			return nullptr;
		}

		// '='
		if (const auto result = context.require(TokenType::Equal); !result) {
			context.had_panic = true;
			return nullptr;
		}

		// <expression>
		auto rhs = parse_expression(context);
		if (!rhs) {
			context.had_panic = true;
			return nullptr;
		}

		// ';'
		if (const auto result = context.require(TokenType::Semicolon); !result) {
			context.had_panic = true;
			return nullptr;
		}

		return AssignNode::create(std::move(lhs), std::move(rhs));
	}

	ASTNode::Dependency Parser::parse_binary(Context& context, ASTNode::Dependency lhs)
	{
		// <binary_expression> ::= <expression> <binary_operator> <expression>

		// <binary_operator>
		static constexpr auto k_binary_operators = {
			// Comparison operators
			TokenType::Greater,
			TokenType::GreaterEqual,
			TokenType::Less,
			TokenType::LessEqual,
			TokenType::DoubleEqual,
			TokenType::BangEqual,

			// Arithmetic operators
			TokenType::Plus,
			TokenType::Minus,
			TokenType::Star,
			TokenType::Slash,
			TokenType::Percent,

			// Logical
			TokenType::DoubleAmpersand,
			TokenType::DoublePipe,
		};
		auto binary_operator = context.require_any(k_binary_operators);
		if (!binary_operator) {
			return nullptr;
		}

		// <expression>
		auto precedence = get_precedence_rule(binary_operator->type()).precedence;
		auto rhs        = parse_expression_with_precedence(context, precedence);
		if (!rhs) {
			return nullptr;
		}

		return BinaryNode::create(std::move(lhs), std::move(rhs), to_node_operator(binary_operator->type()));
	}

	ASTNode::Dependency Parser::parse_for_loop(Context& context)
	{
		// <for_loop> ::= <for> '(' [ <short_variable_declaration> ] ';' [<expression>] ';' [<expression>] ')'
		//                <block_statement>

		// <for>
		if (const auto result = context.require(TokenType::KeywordFor); !result) {
			return nullptr;
		}

		// '('
		if (const auto result = context.require(TokenType::ParenLeft); !result) {
			return nullptr;
		}
		if (const auto result = context.try_match(TokenType::ParenRight); result) {
			// Error! Empty parentheses, expression is needed.
			return nullptr;
		}

		// [Optional] <short_variable_declaration>
		auto initialization = parse_variable_declaration(context, false, true, false);

		// ';'
		if (const auto result = context.try_match(TokenType::Semicolon); !result) {
			return nullptr;
		}

		// [Optional] <expression>
		auto condition = parse_expression(context);

		// ';'
		if (const auto result = context.try_match(TokenType::Semicolon); !result) {
			return nullptr;
		}

		// [Optional] <expression>
		auto update = parse_expression(context);

		// ')'
		if (const auto result = context.require(TokenType::ParenRight); !result) {
			return nullptr;
		}

		// <block_statement>
		auto statements = parse_block_statement(context);

		return ForLoopNode::create(
			std::move(initialization), std::move(condition), std::move(update), std::move(statements));
	}

	ASTNode::Dependency Parser::parse_foreach_loop(Context& context)
	{
		// <foreach_loop> ::= <foreach> '(' <short_variable_declaration> <in> <expression> ')' <block_statement>

		// <foreach>
		if (const auto result = context.require(TokenType::KeywordForeach); !result) {
			return nullptr;
		}

		// '('
		if (const auto result = context.require(TokenType::ParenLeft); !result) {
			return nullptr;
		}
		if (const auto result = context.try_match(TokenType::ParenLeft); result) {
			// Error! Empty parentheses, expression is needed.
			return nullptr;
		}

		// <short_variable_declaration>
		auto variable = parse_variable_declaration(context, false, false, false);
		if (!variable) {
			// Error! Variable declaration is required for 'foreach' loops.
			return nullptr;
		}

		// <in>
		if (const auto result = context.require(TokenType::KeywordIn); !result) {
			return nullptr;
		}

		// <expression>
		auto in_expression = parse_expression(context);
		if (!in_expression) {
			// Error! 'In' Expression is required for 'foreach' loops.
			return nullptr;
		}

		// ')'
		if (const auto result = context.require(TokenType::ParenRight); !result) {
			return nullptr;
		}

		// <block_statement>
		auto statements = parse_block_statement(context);

		return ForeachLoopNode::create(std::move(variable), std::move(in_expression), std::move(statements));
	}

	ASTNode::Dependency Parser::parse_function_declaration(Context& context)
	{
		// <function_declaration> ::= <fn> <name_identifier> [ <parameter_list> ] '::' <type_identifier>
		// <block_statement>

		// <fn>
		if (const auto result = context.require(TokenType::KeywordFn); !result) {
			return nullptr;
		}

		// <name_identifier>
		const auto name_identifier = context.require(TokenType::LiteralIdentifier);
		if (!name_identifier) {
			return nullptr;
		}

		// [Optional] <parameter_list>
		ASTNode::Dependencies parameters;
		if (const auto has_parenthesis = context.try_match(TokenType::ParenLeft); has_parenthesis) {
			// <parameter_list> ::= '(' [ <short_variable_declaration> [ ',' ... ] ] ')'

			// If the next token is not a closing parenthesis we assume that the list of parameters is not empty.
			if (const auto has_parameters = context.try_match(TokenType::ParenRight); !has_parameters) {
				for (;;) {
					if (const auto result = context.try_match(TokenType::EndOfFile); result) {
						// Error! Parentheses were not matched!
						return nullptr;
					}

					auto parameter = parse_variable_declaration(context, false, false, false);
					if (!parameter) {
						// Something went wrong - stop parsing the parentheses.
						context.had_error = true;
						break;
					}
					parameters.push_back(std::move(parameter));
					static constexpr auto k_continuation_types = { TokenType::Comma, TokenType::ParenRight };
					if (const auto result = context.try_match_any(k_continuation_types); !result) {
						// Parameter list was exhausted.
						break;
					}
				}
			}
		}

		// '::'
		if (const auto result = context.require(TokenType::DoubleColon); !result) {
			return nullptr;
		}

		// <type_identifier>
		const auto type_identifier = context.require(TokenType::LiteralIdentifier);
		if (!type_identifier) {
			return nullptr;
		}

		// <block_statement>
		auto statements = parse_block_statement(context);

		return FunctionDeclarationNode::create(name_identifier->value.get<std::string>(),
		                                       type_identifier->value.get<std::string>(),
		                                       std::move(parameters),
		                                       std::move(statements));
	}

	ASTNode::Dependency Parser::parse_literal(Context& context)
	{
		// <literal> ::= <integer_literal> | <float_literal> | <string_literal>

		auto value_token = context.require_any(k_literal_types);
		if (!value_token) {
			return nullptr;
		}
		if (value_token->type() == TokenType::KeywordTrue || value_token->type() == TokenType::KeywordFalse) {
			return LiteralNode::create(Value{ static_cast<bool>(value_token->type() == TokenType::KeywordTrue) });
		}
		return LiteralNode::create(std::move(value_token->value));
	}

	ASTNode::Dependency Parser::parse_if(Context& context)
	{
		// <if_statement> ::= <if> '(' <expression> ')' <block_statement> [ <else> <block_statement> ]

		// <if>
		if (const auto result = context.require(TokenType::KeywordIf); !result) {
			return nullptr;
		}

		// '('
		if (const auto result = context.require(TokenType::ParenLeft); !result) {
			return nullptr;
		}

		// <expression>
		auto condition = parse_expression(context);
		if (!condition) {
			return nullptr;
		}

		// ')'
		if (const auto result = context.require(TokenType::ParenRight); !result) {
			return nullptr;
		}

		// <block statement>
		auto if_statements = parse_block_statement(context);

		// [ <else> <block_statement> ]
		ASTNode::Dependencies else_statements;
		if (const auto result = context.try_match(TokenType::KeywordElse); result) {
			else_statements = parse_block_statement(context);
		}

		return IfNode::create(std::move(condition), std::move(if_statements), std::move(else_statements));
	}

	ASTNode::Dependency Parser::parse_struct_declaration(Context& context)
	{
		// <struct_declaration> ::= <struct> <name_identifier> '{' <short_variable_declaration> [ ',' ... ] '}'

		// <struct>
		if (const auto result = context.require(TokenType::KeywordStruct); !result) {
			return nullptr;
		}

		// <name_identifier>
		const auto name_identifier = context.require(TokenType::LiteralIdentifier);
		if (!name_identifier) {
			return nullptr;
		}

		// '{'
		if (const auto result = context.require(TokenType::BraceLeft); !result) {
			return nullptr;
		}

		ASTNode::Dependencies statements;
		for (;;) {
			if (const auto result = context.try_match(TokenType::EndOfFile); result) {
				// Error! Parentheses were not matched!
				return nullptr;
			}

			auto statement = parse_variable_declaration(context, false, false, false);
			if (!statement) {
				// Something went wrong - stop parsing the parentheses.
				context.had_error = true;
				break;
			}
			statements.push_back(std::move(statement));
			static constexpr auto k_continuation_types = { TokenType::Comma, TokenType::ParenRight };
			if (const auto result = context.try_match_any(k_continuation_types); !result) {
				// Parameter list was exhausted.
				break;
			}
		}

		// }
		if (const auto result = context.require(TokenType::BraceRight); !result) {
			return nullptr;
		}

		return StructDeclarationNode::create(name_identifier->value.get<std::string>(), std::move(statements));
	}

	ASTNode::Dependency Parser::parse_variable_declaration(Context& context,
	                                                       bool     require_keyword,
	                                                       bool     require_expression,
	                                                       bool     require_semicolon)
	{
		// <variable_declaration> ::= <let> [ <mut> ] <name_identifier> ':' <type_identifier> '=' <expression> ';'

		// <let>
		if (const auto result = context.require(TokenType::KeywordLet); require_keyword && !result) {
			return nullptr;
		}

		// [ <mut> ]
		const bool is_mutable = require_keyword && context.try_match(TokenType::KeywordMut);

		// <name_identifier>
		const auto name_identifier = context.require(TokenType::LiteralIdentifier);
		if (!name_identifier) {
			return nullptr;
		}

		// ':'
		if (const auto result = context.require(TokenType::Colon); !result) {
			return nullptr;
		}

		// <type_identifier>
		const auto type_identifier = context.require(TokenType::LiteralIdentifier);
		if (!type_identifier) {
			return nullptr;
		}

		ASTNode::Dependency expression = nullptr;
		if (require_expression) {
			// '='
			if (const auto result = context.require(TokenType::Equal); !result) {
				return nullptr;
			}

			// <expression>
			expression = parse_expression(context);
			if (!expression) {
				return nullptr;
			}
		}

		// ';'
		if (require_semicolon) {
			if (const auto result = context.try_match(TokenType::Semicolon); !result) {
				return nullptr;
			}
		}

		return VariableDeclarationNode::create(name_identifier->value.get<std::string>(),
		                                       type_identifier->value.get<std::string>(),
		                                       std::move(expression),
		                                       is_mutable);
	}

	ASTNode::Dependency Parser::parse_suffix(Context& context) { return nullptr; }

	ASTNode::Dependency Parser::parse_unary(Context& context)
	{
		// TODO
		return nullptr;
	}

	ASTNode::Dependency Parser::parse_while_loop(Context& context)
	{
		// <while_loop_statement> ::=  while '(' <expression> ')' <block_statement>
		//                          |  while <block_statement>

		// <while>
		if (const auto result = context.require(TokenType::KeywordWhile); !result) {
			return nullptr;
		}

		// '('
		ASTNode::Dependency condition = nullptr;
		if (const auto has_parentheses = context.try_match(TokenType::ParenLeft); has_parentheses) {
			if (const auto result = context.try_match(TokenType::ParenRight); result) {
				// Error! Empty parentheses, expression is needed.
				return nullptr;
			}

			// <expression>
			condition = parse_expression(context);
			if (!condition) {
				return nullptr;
			}

			// ')'
			if (const auto result = context.require(TokenType::ParenRight); !result) {
				return nullptr;
			}
		} else {
			// While loops without parentheses have implicit `true` condition.
			condition = LiteralNode::create(Value(true));
		}

		// <block_statement>
		auto statements = parse_block_statement(context);

		return ForLoopNode::create(nullptr, std::move(condition), nullptr, std::move(statements));
	}

	ASTNode::Dependencies Parser::parse_block_statement(Context& context)
	{
		// <block_statement> ::= '{' [ <statements> ] '}'

		if (const auto result = context.require(TokenType::BraceLeft); !result) {
			// TODO: Should this return empty?
			return {};
		}

		ASTNode::Dependencies statements;
		while (!context.try_match(TokenType::BraceRight)) {
			auto statement = parse_statement(context);
			if (!statement) {
				// TODO: Should this return empty?
				return {};
			}
			statements.push_back(std::move(statement));
		}

		return statements;
	}

	Parser::PrecedenceRule Parser::get_precedence_rule(TokenType type) const noexcept
	{
		if (std::ranges::find(k_literal_types, type) != std::end(k_literal_types)) {
			return { Parser::Precedence::None, &Parser::parse_literal, nullptr };
		}

		if (std::ranges::find(k_compare_types, type) != std::end(k_compare_types)) {
			return { Parser::Precedence::Compare, nullptr, &Parser::parse_binary, nullptr };
		}

		switch (type) {
			case TokenType::Minus:
				return { Parser::Precedence::Additive, &Parser::parse_unary, &Parser::parse_binary, nullptr };
			case TokenType::Plus:
				return { Parser::Precedence::Additive, nullptr, &Parser::parse_binary, nullptr };
			case TokenType::DoublePlus:
				return { Parser::Precedence::Call, &Parser::parse_unary, nullptr, &Parser::parse_suffix };
			case TokenType::Star:
			case TokenType::Slash:
				return { Parser::Precedence::Multiplicative, nullptr, &Parser::parse_binary, nullptr };
			default:
				break;
		}

		return PrecedenceRule{ Precedence::None, nullptr, nullptr };  // No precedence.
	}

	Parser::Context::Context(std::span<const Token> tokens) : _tokens(tokens), _current_index(0) {}

	std::optional<Token> Parser::Context::require(TokenType type)
	{
		if (current_token()->is_type(type)) {
			return advance();
		}
		return std::nullopt;
	}

	std::optional<Token> Parser::Context::require_any(std::span<const TokenType> types)
	{
		for (const auto type : types) {
			if (const auto result = require(type); result) {
				return result;
			}
		}
		return std::nullopt;
	}

	bool Parser::Context::try_match(TokenType type)
	{
		if (current_token()->is_type(type)) {
			_current_index++;
			return true;
		}

		return false;
	}

	bool Parser::Context::try_match_any(std::span<const TokenType> types)
	{
		for (const auto type : types) {
			if (const auto result = try_match(type); result) {
				return result;
			}
		}
		return false;
	}

	const Token& Parser::Context::advance() noexcept
	{
		_current_index++;
		if (_current_index > _tokens.size()) {
			return _tokens.back();
		}
		return _tokens[_current_index - 1];
	}

	void Parser::Context::synchronize()
	{
		static constexpr auto k_synchronization_tokens = {
			TokenType::KeywordFn,     TokenType::KeywordLet,   TokenType::KeywordIf,
			TokenType::KeywordFor,    TokenType::KeywordWhile, TokenType::KeywordReturn,
			TokenType::KeywordStruct, TokenType::BraceLeft /* Scope */
		};

		while (!try_match(TokenType::EndOfFile)) {
			if (try_match_any(k_synchronization_tokens)) {
				break;  // Synchronized.
			}
			_current_index++;
		}
		had_panic = false;
	}

	const Token* Parser::Context::previous_token() const noexcept { return &_tokens[_current_index - 1]; }

	const Token* Parser::Context::current_token() const noexcept { return &_tokens[_current_index]; }

	const Token* Parser::Context::next_token() const noexcept { return &_tokens[_current_index + 1]; }
}  // namespace soul::parser
