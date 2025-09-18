#include "parser/parser.h"

#include "ast/ast.h"
#include "ast/nodes/binary.h"
#include "ast/nodes/block.h"
#include "ast/nodes/cast.h"
#include "ast/nodes/error.h"
#include "ast/nodes/for_loop.h"
#include "ast/nodes/foreach_loop.h"
#include "ast/nodes/function_declaration.h"
#include "ast/nodes/if.h"
#include "ast/nodes/literal.h"
#include "ast/nodes/module.h"
#include "ast/nodes/struct_declaration.h"
#include "ast/nodes/unary.h"
#include "ast/nodes/variable_declaration.h"

#include <algorithm>
#include <unordered_map>

namespace soul::parser
{
	using namespace ast::nodes;
	using namespace ast;

	static constexpr std::array k_literal_types
		= { Token::Type::LiteralFloat,  Token::Type::LiteralIdentifier, Token::Type::LiteralInteger,
		    Token::Type::LiteralString, Token::Type::KeywordTrue,       Token::Type::KeywordFalse };

	static constexpr std::array k_compare_types = { Token::Type::SymbolLess,
		                                            Token::Type::SymbolLessEqual,
		                                            Token::Type::SymbolGreater,
		                                            Token::Type::SymbolGreaterEqual };

	enum class Parser::Precedence : u8
	{
		None,
		Assign,          // =
		Or,              // ||
		And,             // &&
		Equal,           // == !=
		Compare,         // < > <= >=
		Additive,        // + -
		Multiplicative,  // * /
		Unary,           // ! -
		Call,            // ()
	};

	struct Parser::PrecedenceRule
	{
		public:
		typedef ASTNode::Dependency (Parser::*PrefixFn)();
		typedef ASTNode::Dependency (Parser::*InfixFn)(ASTNode::Dependency);
		typedef ASTNode::Dependency (Parser::*SuffixFn)();

		public:
		Precedence precedence = Precedence::None;
		PrefixFn   prefix     = nullptr;
		InfixFn    infix      = nullptr;
		SuffixFn   suffix     = nullptr;
	};

	Parser::Parser(std::string_view module_name, std::span<const Token> tokens)
		: _tokens(tokens), _current_token(_tokens.begin()), _module_name(module_name)
	{
	}

	ast::ASTNode::Dependency Parser::parse(std::string_view module_name, std::span<const Token> tokens)
	{
		return Parser{ module_name, tokens }.parse();
	}

	ast::ASTNode::Dependency Parser::parse()
	{
		if (_tokens.empty()) {
			return ModuleNode::create(std::string(_module_name), {});
		}

		ASTNode::Dependencies statements{};
		for (const auto& token : _tokens) {
			if (token.type == Token::Type::SpecialError) {
				statements.emplace_back(ErrorNode::create(ErrorNode::Message{ token.data }));
			}
		}
		if (!statements.empty()) {
			return ModuleNode::create(std::string(_module_name), std::move(statements));
		}

		while (_current_token != std::end(_tokens)) {
			statements.emplace_back(parse_statement());
		}
		return ModuleNode::create(std::string(_module_name), std::move(statements));
	}

	ASTNode::Dependency Parser::parse_statement()
	{
		// Statements
		switch (_current_token->type) {
			case Token::Type::KeywordFn:
				return parse_function_declaration();
			case Token::Type::KeywordFor:
				return parse_for_loop();
			case Token::Type::KeywordIf:
				return parse_if();
			case Token::Type::KeywordLet:
				return parse_variable_declaration();
			case Token::Type::KeywordStruct:
				return parse_struct_declaration();
			case Token::Type::KeywordWhile:
				return parse_while_loop();
			case Token::Type::SymbolBraceLeft:
				return BlockNode::create(parse_block_statement());
			default:
				break;
		}

		return parse_expression();
	}

	ASTNode::Dependency Parser::parse_expression()
	{
		// NOTE: Starting precedence has to be at least one higher than no precedence.
		return parse_expression(static_cast<Parser::Precedence>(std::to_underlying(Parser::Precedence::None) + 1));
	}

	ASTNode::Dependency Parser::parse_expression(Parser::Precedence precedence)
	{
		auto prefix_rule = precedence_rule(_current_token->type).prefix;
		if (!prefix_rule) [[unlikely]] {
			return create_error(std::format("[INTERNAL] no prefix precedence rule for '{}' was specified.",
			                                Token::internal_name(current_token_or_default().type)));
		}

		auto prefix_expression = (this->*prefix_rule)();

		while (precedence <= precedence_rule(_current_token->type).precedence) {
			auto infix_rule = precedence_rule(_current_token->type).infix;
			if (!infix_rule) [[unlikely]] {
				return create_error(std::format("[INTERNAL] no infix precedence rule for '{}' was specified.",
				                                Token::internal_name(current_token_or_default().type)));
			}
			prefix_expression = (this->*infix_rule)(std::move(prefix_expression));
		}

		return prefix_expression;
	}

	ASTNode::Dependency Parser::parse_binary(ASTNode::Dependency lhs)
	{
		// <binary_expression> ::= <expression> <binary_operator> <expression>

		// <binary_operator>
		static constexpr std::array k_binary_operators = {
			// Comparison operators
			Token::Type::SymbolGreater,
			Token::Type::SymbolGreaterEqual,
			Token::Type::SymbolLess,
			Token::Type::SymbolLessEqual,
			Token::Type::SymbolEqualEqual,
			Token::Type::SymbolBangEqual,

			// Arithmetic operators
			Token::Type::SymbolPlus,
			Token::Type::SymbolMinus,
			Token::Type::SymbolStar,
			Token::Type::SymbolSlash,
			Token::Type::SymbolPercent,

			// Logical
			Token::Type::SymbolAmpersandAmpersand,
			Token::Type::SymbolPipePipe,
		};
		auto binary_operator = require(k_binary_operators);
		if (!binary_operator) {
			return create_error(
				std::format("expected binary operator, but got: '{}'", std::string(current_token_or_default().data)));
		}

		// <expression>
		auto precedence = precedence_rule(binary_operator->type).precedence;
		auto rhs        = parse_expression(precedence);

		return BinaryNode::create(std::move(lhs), std::move(rhs), ASTNode::as_operator(binary_operator->type));
	}

	ASTNode::Dependency Parser::parse_cast()
	{
		// <cast_expression> ::= <keyword_cast> '<' <identifier> '>' '(' <expression> ')'

		// <keyword_cast>
		if (!require(Token::Type::KeywordCast)) {
			return create_error(std::format("expected '{}' keyword, but got: '{}'",
			                                Token::name(Token::Type::KeywordCast),
			                                std::string(current_token_or_default().data)));
		}

		// '<'
		if (!require(Token::Type::SymbolLess)) {
			return create_error(std::format("expected '{}', but got: '{}'",
			                                Token::name(Token::Type::SymbolLess),
			                                std::string(current_token_or_default().data)));
		}

		// <identifier>
		auto type_identifier = require(Token::Type::LiteralIdentifier);
		if (!type_identifier) {
			return create_error(
				std::format("expected type identifier, but got: '{}'", std::string(current_token_or_default().data)));
		}

		// '>'
		if (!require(Token::Type::SymbolGreater)) {
			return create_error(std::format("expected '{}', but got: '{}'",
			                                Token::name(Token::Type::SymbolGreater),
			                                std::string(current_token_or_default().data)));
		}

		// '('
		if (!require(Token::Type::SymbolParenLeft)) {
			return create_error(std::format("expected '{}', but got: '{}'",
			                                Token::name(Token::Type::SymbolParenLeft),
			                                std::string(current_token_or_default().data)));
		}

		// <expression>
		auto expression = parse_expression();

		// ')'
		if (!require(Token::Type::SymbolParenRight)) {
			return create_error(std::format("expected '{}', but got: '{}'",
			                                Token::name(Token::Type::SymbolParenRight),
			                                std::string(current_token_or_default().data)));
		}

		return CastNode::create(std::move(expression), std::string(type_identifier->data));
	}

	ASTNode::Dependency Parser::parse_for_loop()
	{
		// <for_loop> ::= <keyword_for> '(' [<expression>] ';' [ <expression> ] ';' [ <expression> ] ')'
		// <block_statement>

		// <keyword_for>
		if (!require(Token::Type::KeywordFor)) {
			return create_error(std::format("expected '{}' keyword, but got: '{}'",
			                                Token::name(Token::Type::KeywordFor),
			                                std::string(current_token_or_default().data)));
		}

		// '('
		if (!require(Token::Type::SymbolParenLeft)) {
			return create_error(std::format("expected '{}', but got: '{}'",
			                                Token::name(Token::Type::SymbolParenLeft),
			                                std::string(current_token_or_default().data)));
		}

		// [Optional] <expression>
		ASTNode::Dependency initialization = nullptr;
		if (!match(Token::Type::SymbolSemicolon)) {
			initialization = parse_parameter_declaration();

			// ';'
			if (!require(Token::Type::SymbolSemicolon)) {
				return create_error(std::format("expected '{}', but got: '{}'",
				                                Token::name(Token::Type::SymbolSemicolon),
				                                std::string(current_token_or_default().data)));
			}
		}

		// [Optional] <expression>
		ASTNode::Dependency condition = nullptr;
		if (!match(Token::Type::SymbolSemicolon)) {
			condition = parse_expression();

			// ';'
			if (!require(Token::Type::SymbolSemicolon)) {
				return create_error(std::format("expected '{}', but got: '{}'",
				                                Token::name(Token::Type::SymbolSemicolon),
				                                std::string(current_token_or_default().data)));
			}
		}

		// [Optional] <expression>
		ASTNode::Dependency update = nullptr;
		if (!match(Token::Type::SymbolParenRight)) {
			update = parse_expression();

			// ')'
			if (!require(Token::Type::SymbolParenRight)) {
				return create_error(std::format("expected '{}', but got: '{}'",
				                                Token::name(Token::Type::SymbolParenRight),
				                                std::string(current_token_or_default().data)));
			}
		}

		// <block_statement>
		auto statements = parse_block_statement();

		return ForLoopNode::create(std::move(initialization),
		                           std::move(condition),
		                           std::move(update),
		                           BlockNode::create(std::move(statements)));
	}

	ASTNode::Dependency Parser::parse_function_declaration()
	{
		// <function_declaration> ::= <keyword_fn> <identifier> [ <parameter_declaration>, ... ] '::' <identifier>
		// <block_statement>

		// <keyword_fn>
		if (!require(Token::Type::KeywordFn)) {
			return create_error(std::format("expected '{}' keyword, but got: '{}'",
			                                Token::name(Token::Type::KeywordFn),
			                                std::string(current_token_or_default().data)));
		}

		// <identifier>
		auto name_identifier = require(Token::Type::LiteralIdentifier);
		if (!name_identifier) {
			return create_error(std::format("expected function identifier, but got: '{}'",
			                                std::string(current_token_or_default().data)));
		}

		// [Optional] '(' <parameter_list> ')'
		ASTNode::Dependencies parameters{};
		if (match(Token::Type::SymbolParenLeft)) {
			const bool parenthesis_next = current_token_or_default().type == Token::Type::SymbolParenRight;
			if (!parenthesis_next) {
				do {
					parameters.emplace_back(parse_parameter_declaration());
				} while (match(Token::Type::SymbolComma));
			}

			if (!require(Token::Type::SymbolParenRight)) {
				return create_error(std::format("expected '{}', but got: '{}'",
				                                Token::name(Token::Type::SymbolParenRight),
				                                std::string(current_token_or_default().data)));
			}
		}

		// '::'
		if (!require(Token::Type::SymbolColonColon)) {
			return create_error(std::format("expected type separator '{}', but got: '{}'",
			                                Token::name(Token::Type::SymbolColonColon),
			                                std::string(current_token_or_default().data)));
		}

		// <identifier>
		auto type_identifier = require(Token::Type::LiteralIdentifier);
		if (!type_identifier) {
			return create_error(
				std::format("expected type identifier, but got: '{}'", std::string(current_token_or_default().data)));
		}

		auto statements = parse_block_statement();

		return FunctionDeclarationNode::create(std::string(name_identifier->data),
		                                       std::string(type_identifier->data),
		                                       std::move(parameters),
		                                       BlockNode::create(std::move(statements)));
	}

	ASTNode::Dependency Parser::parse_if()
	{
		// <if_statement> ::= <keyword_if> '(' <expression> ')' <block_statement> [ <keyword_else> <block_statement> ]

		// <keyword_if>
		if (!require(Token::Type::KeywordIf)) {
			return create_error(std::format("expected '{}' keyword, but got: '{}'",
			                                Token::name(Token::Type::KeywordIf),
			                                std::string(current_token_or_default().data)));
		}

		// '('
		if (!require(Token::Type::SymbolParenLeft)) {
			return create_error(std::format("expected '{}', but got: '{}'",
			                                Token::name(Token::Type::SymbolParenLeft),
			                                std::string(current_token_or_default().data)));
		}

		// <expression>
		auto condition = parse_expression();

		// ')'
		if (!require(Token::Type::SymbolParenRight)) {
			return create_error(std::format("expected '{}', but got: '{}'",
			                                Token::name(Token::Type::SymbolParenRight),
			                                std::string(current_token_or_default().data)));
		}

		// <block_statement>
		auto true_statements = parse_block_statement();

		// [ <keyword_else> <block_statement> ]
		ASTNode::Dependencies false_statements{};
		if (match(Token::Type::KeywordElse)) {
			false_statements = parse_block_statement();
		}

		return IfNode::create(std::move(condition),
		                      BlockNode::create(std::move(true_statements)),
		                      BlockNode::create(std::move(false_statements)));
	}

	ASTNode::Dependency Parser::parse_literal()
	{
		// <literal> ::= <integer_literal>  | <float_literal> | <string_literal> | <keyword_true> | <keyword_false>

		const auto& token = require(std::array{ Token::Type::KeywordFalse,
		                                        Token::Type::KeywordTrue,
		                                        Token::Type::LiteralFloat,
		                                        Token::Type::LiteralIdentifier,
		                                        Token::Type::LiteralInteger,
		                                        Token::Type::LiteralString });
		if (!token) {
			return create_error(std::format("expected literal expression, but got: '{}'",
			                                Token::name(current_token_or_default().type)));
		}

		LiteralNode::Type literal_type{};
		Value             value{};
		if (token->type == Token::Type::LiteralFloat) {
			f64        v{};
			const auto result = std::from_chars(std::begin(token->data), std::end(token->data), v);
			if (result.ec != std::errc{}) {
				return create_error(std::format("failed to parse float expression, because: '{}'",
				                                std::make_error_condition(result.ec).message()));
			}
			value = Value{ v };

			literal_type = v <= std::numeric_limits<f32>::lowest() || v >= std::numeric_limits<f32>::max()
			                 ? LiteralNode::Type::Float64
			                 : LiteralNode::Type::Float32;
		}

		if (token->type == Token::Type::LiteralInteger) {
			i64        v{};
			const auto result = std::from_chars(std::begin(token->data), std::end(token->data), v);
			if (result.ec != std::errc{}) {
				return create_error(std::format("failed to parse integer expression, because: '{}'",
				                                std::make_error_condition(result.ec).message()));
			}
			value = Value{ v };

			literal_type = v <= std::numeric_limits<i32>::lowest() || v >= std::numeric_limits<i32>::max()
			                 ? LiteralNode::Type::Int64
			                 : LiteralNode::Type::Int32;
		}

		if (token->type == Token::Type::LiteralString) {
			literal_type = LiteralNode::Type::String;
			value        = Value{ std::string(token->data) };
		}

		if (token->type == Token::Type::LiteralIdentifier) {
			literal_type = LiteralNode::Type::Identifier;
			value        = Value{ std::string(token->data) };
		}

		if (token->type == Token::Type::KeywordTrue) {
			literal_type = LiteralNode::Type::Boolean;
			value        = Value{ true };
		}

		if (token->type == Token::Type::KeywordFalse) {
			literal_type = LiteralNode::Type::Boolean;
			value        = Value{ false };
		}
		return LiteralNode::create(std::move(value), literal_type);
	}

	ASTNode::Dependency Parser::parse_struct_declaration()
	{
		// <struct_declaration> ::= <keyword_struct> <identifier> '{' <parameter_declaration> [',' ... ] '}'

		// <keyword_struct>
		if (!require(Token::Type::KeywordStruct)) {
			return create_error(std::format("expected '{}' keyword, but got: '{}'",
			                                Token::name(Token::Type::KeywordStruct),
			                                std::string(current_token_or_default().data)));
		}

		// <identifier>
		auto name_identifier = require(Token::Type::LiteralIdentifier);
		if (!name_identifier) {
			return create_error(
				std::format("expected struct identifier, but got: '{}'", std::string(current_token_or_default().data)));
		}

		// '{'
		if (!require(Token::Type::SymbolBraceLeft)) {
			return create_error(std::format("expected '{}', but got: '{}'",
			                                Token::name(Token::Type::SymbolBraceLeft),
			                                std::string(current_token_or_default().data)));
		}

		ASTNode::Dependencies parameters{};
		if (const auto current_type = current_token_or_default().type; current_type != Token::Type::SymbolBraceRight) {
			if (current_type == Token::Type::SpecialEndOfFile) {
				return create_error(std::format("expected '{}', but got: '{}'",
				                                Token::name(Token::Type::SymbolBraceRight),
				                                std::string(current_token_or_default().data)));
			}

			while (!match(Token::Type::SymbolBraceRight)) {
				parameters.emplace_back(parse_parameter_declaration());

				// ','
				if (current_token_or_default().type != Token::Type::SymbolBraceRight
				    && !require(Token::Type::SymbolComma)) {
					return create_error(std::format("expected '{}', but got: '{}'",
					                                Token::name(Token::Type::SymbolComma),
					                                std::string(current_token_or_default().data)));
				}
			}

			// '}'
			const auto previous_token = peek(-1);
			if (!previous_token || previous_token->type != Token::Type::SymbolBraceRight) {
				return create_error(std::format("expected '{}', but got: '{}'",
				                                Token::name(Token::Type::SymbolBraceRight),
				                                std::string(current_token_or_default().data)));
			}
		} else {
			// '}'
			if (!require(Token::Type::SymbolBraceRight)) {
				return create_error(std::format("expected '{}', but got: '{}'",
				                                Token::name(Token::Type::SymbolBraceRight),
				                                std::string(current_token_or_default().data)));
			}
		};

		return StructDeclarationNode::create(std::string(name_identifier->data), std::move(parameters));
	}

	ASTNode::Dependency Parser::parse_unary() { return create_error("Parser::parse_unary is not implemented yet."); }

	ASTNode::Dependency Parser::parse_variable_declaration()
	{
		// <variable_declaration> ::= <keyword_let> [ <keyword_mut> ] <identifier> ':' <identifier> '=' <expression> ';'

		// <keyword_let>
		if (!require(Token::Type::KeywordLet)) {
			return create_error(std::format("expected '{}' keyword, but got: '{}'",
			                                Token::name(Token::Type::KeywordLet),
			                                std::string(current_token_or_default().data)));
		}

		// [Optional] <keyword_mut>
		const bool is_mutable = match(Token::Type::KeywordMut);

		// <identifier>
		auto name_identifier = require(Token::Type::LiteralIdentifier);
		if (!name_identifier) {
			return create_error(std::format("expected variable identifier, but got: '{}'",
			                                std::string(current_token_or_default().data)));
		}

		// ':'
		if (!require(Token::Type::SymbolColon)) {
			return create_error(std::format("expected '{}', but got: '{}'",
			                                Token::name(Token::Type::SymbolColon),
			                                std::string(current_token_or_default().data)));
		}

		// <identifier>
		auto type_identifier = require(Token::Type::LiteralIdentifier);
		if (!type_identifier) {
			return create_error(
				std::format("expected type identifier, but got: '{}'", std::string(current_token_or_default().data)));
		}

		// '='
		if (!require(Token::Type::SymbolEqual)) {
			return create_error(std::format("expected '{}', but got: '{}'",
			                                Token::name(Token::Type::SymbolEqual),
			                                std::string(current_token_or_default().data)));
		}

		// <expression>
		auto expression = parse_expression();

		// ';'
		if (!require(Token::Type::SymbolSemicolon)) {
			return create_error(std::format("expected '{}', but got: '{}'",
			                                Token::name(Token::Type::SymbolSemicolon),
			                                std::string(current_token_or_default().data)));
		}

		return VariableDeclarationNode::create(
			std::string(name_identifier->data), std::string(type_identifier->data), std::move(expression), is_mutable);
	}

	ASTNode::Dependency Parser::parse_while_loop()
	{
		// <while_loop> ::= <keyword_while> [ '(' <expression> ')' ] <block_statement>

		// <keyword_while>
		if (!require(Token::Type::KeywordWhile)) {
			return create_error(std::format("expected '{}' keyword, but got: '{}'",
			                                Token::name(Token::Type::KeywordWhile),
			                                std::string(current_token_or_default().data)));
		}

		// [Optional] '(' <expression> ')'
		ASTNode::Dependency condition{};
		if (match(Token::Type::SymbolParenLeft)) {
			// <expression>
			condition = parse_expression();

			// ')'
			if (!require(Token::Type::SymbolParenRight)) {
				return create_error(std::format("expected '{}', but got: '{}'",
				                                Token::name(Token::Type::SymbolParenRight),
				                                std::string(current_token_or_default().data)));
			}
		} else {
			condition = LiteralNode::create(Value{ true }, LiteralNode::Type::Boolean);
		}

		// <block_statement>
		auto statements = parse_block_statement();

		return ForLoopNode::create(nullptr, std::move(condition), nullptr, BlockNode::create(std::move(statements)));
	}

	ASTNode::Dependencies Parser::parse_block_statement()
	{
		// <block_statement> ::= '{' [ <statement> ... ] '}'

		ASTNode::Dependencies statements{};
		if (!require(Token::Type::SymbolBraceLeft)) {
			statements.emplace_back(create_error(std::format("expected '{}', but got: '{}'",
			                                                 Token::name(Token::Type::SymbolBraceLeft),
			                                                 std::string(current_token_or_default().data))));
			return statements;
		}

		while (!match(Token::Type::SymbolBraceRight)) {
			if (current_token_or_default().type == Token::Type::SpecialEndOfFile) {
				break;
			}

			statements.emplace_back(parse_statement());
		}

		const auto previous_token = peek(-1);
		if (!previous_token || previous_token->type != Token::Type::SymbolBraceRight) {
			statements.emplace_back(create_error(std::format("expected '{}', but got: '{}'",
			                                                 Token::name(Token::Type::SymbolBraceRight),
			                                                 std::string(current_token_or_default().data))));
			return statements;
		}

		return statements;
	}

	ASTNode::Dependency Parser::parse_parameter_declaration()
	{
		// <parameter_declaration> ::= <identifier> ':' <identifier> [ '=' <expression> ]

		// <identifier>
		auto name_identifier = require(Token::Type::LiteralIdentifier);
		if (!name_identifier) {
			return create_error(
				std::format("expected name identifier, but got: '{}'", std::string(current_token_or_default().data)));
		}

		// ':'
		if (!require(Token::Type::SymbolColon)) {
			return create_error(std::format("expected '{}', but got: '{}'",
			                                Token::name(Token::Type::SymbolBraceLeft),
			                                std::string(current_token_or_default().data)));
		}

		// <identifier>
		auto type_identifier = require(Token::Type::LiteralIdentifier);
		if (!type_identifier) {
			return create_error(
				std::format("expected type identifier, but got: '{}'", std::string(current_token_or_default().data)));
		}

		// [ '=' <expression> ]
		ASTNode::Dependency expression = nullptr;
		if (match(Token::Type::SymbolEqual)) {
			expression = parse_expression();
		}

		return VariableDeclarationNode::create(
			std::string(name_identifier->data), std::string(type_identifier->data), std::move(expression), false);
	}

	ASTNode::Dependency Parser::create_error(ErrorNode::Message error_message)
	{
		static constexpr std::array k_synchronization_tokens = {
			Token::Type::KeywordElse,      Token::Type::KeywordFn,        Token::Type::KeywordFor,
			Token::Type::KeywordIf,        Token::Type::KeywordLet,       Token::Type::KeywordNative,
			Token::Type::KeywordStruct,    Token::Type::KeywordWhile,     Token::Type::SymbolSemicolon,
			Token::Type::SymbolBraceRight, Token::Type::SymbolParenRight,
		};
		while (_current_token != std::end(_tokens)) {
			if (std::ranges::contains(k_synchronization_tokens, _current_token->type)) {
				_current_token++;
				break;  // Synchronized.
			}
			_current_token++;
		}

		return ErrorNode::create(std::move(error_message));
	}

	std::optional<Token> Parser::require(Token::Type type)
	{
		if (_current_token == std::end(_tokens) || _current_token->type != type) {
			return std::nullopt;
		}
		return *_current_token++;
	}

	std::optional<Token> Parser::require(std::span<const Token::Type> types)
	{
		if (_current_token == std::end(_tokens)) {
			return std::nullopt;
		}

		for (const auto& type : types) {
			if (_current_token->type == type) {
				return *_current_token++;
			}
		}
		return std::nullopt;
	}

	std::optional<Token> Parser::peek(std::ptrdiff_t n)
	{
		if ((_current_token + n) != std::end(_tokens)) {
			return *(_current_token + n);
		}
		return std::nullopt;
	}

	bool Parser::match(Token::Type type)
	{
		if (_current_token == std::end(_tokens) || _current_token->type != type) {
			return false;
		}
		_current_token++;
		return true;
	}

	Parser::PrecedenceRule Parser::precedence_rule(Token::Type type) const noexcept
	{
		if (std::ranges::contains(k_literal_types, type)) {
			return { Precedence::None, &Parser::parse_literal, nullptr, nullptr };
		}

		if (std::ranges::contains(k_compare_types, type)) {
			return { Precedence::Compare, nullptr, &Parser::parse_binary, nullptr };
		}

		switch (type) {
			case Token::Type::KeywordCast:
				return { Precedence::None, &Parser::parse_cast, nullptr, nullptr };
			case Token::Type::SymbolMinus:
				return { Parser::Precedence::Additive, &Parser::parse_unary, &Parser::parse_binary, nullptr };
			case Token::Type::SymbolPlus:
				return { Parser::Precedence::Additive, nullptr, &Parser::parse_binary, nullptr };
			case Token::Type::SymbolStar:
			case Token::Type::SymbolSlash:
				return { Parser::Precedence::Multiplicative, nullptr, &Parser::parse_binary, nullptr };
			default:
				break;
		}

		return { Precedence::None, nullptr, nullptr, nullptr };  // No precedence.
	}

	Token Parser::current_token_or_default() const noexcept
	{
		if (_current_token == std::end(_tokens)) {
			auto last_token = _tokens.back();
			return Token{
				.type     = Token::Type::SpecialEndOfFile,
				.data     = Token::internal_name(Token::Type::SpecialEndOfFile),
				.location = SourceLocation{ last_token.location.row,
                                           static_cast<u32>(last_token.location.column + last_token.data.size()) }
			};
		}
		return *_current_token;
	}

}  // namespace soul::parser
