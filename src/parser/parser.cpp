#include "parser/parser.h"

#include "ast/ast_operator.h"
#include "ast/nodes/assign.h"
#include "ast/nodes/binary.h"
#include "ast/nodes/function_declaration.h"
#include "ast/nodes/literal.h"
#include "ast/nodes/loop.h"
#include "ast/nodes/struct_declaration.h"
#include "ast/nodes/unary.h"
#include "ast/nodes/variable_declaration.h"

namespace soul
{
	ASTNode::Dependency Parser::parse(std::span<const Token> tokens)
	{
		if (tokens.empty()) {
			return nullptr;
		}

		auto parsing_context = Context{
			.tokens        = tokens,
			.current_index = 0,
			.had_panic     = false,
			.had_error     = false,
			.diagnostics   = {},
		};

		ASTNode::Dependencies statements;
		while (!match(parsing_context, TokenType::EndOfFile)) {
			auto statement = parse_statement(parsing_context);
			if (statement) {
				statements.push_back(std::move(statement));
			}

			if (parsing_context.had_panic) {
				synchronize(parsing_context);
			}
		}
	}

	ASTNode::Dependency Parser::parse_statement(Context& context)
	{
		const auto type = context.tokens[context.current_index].type();
		switch (type) {
			case TokenType::KeywordFn:
				return parse_function_declaration(context);
			case TokenType::KeywordFor:
				return nullptr;  // TODO
			case TokenType::KeywordIf:
				return nullptr;  // TODO
			case TokenType::KeywordLet:
				return parse_variable_declaration(context);
			case TokenType::KeywordStruct:
				return parse_struct_declaration(context);
			case TokenType::KeywordWhile:
				return nullptr;  // TODO
			default:
				break;
		}
		return nullptr;  // TODO
	}

	ASTNode::Dependency Parser::parse_expression(Context& context) {}

	ASTNode::Dependency Parser::parse_assign(Context& context)
	{
		// <assign_statement> ::= <expression> = <expression>;

		// <expression>
		auto lhs = parse_expression(context);
		if (!lhs) {
			context.had_panic = true;
			return nullptr;
		}

		// =
		if (const auto result = require(context, TokenType::Equal); !result) {
			context.had_panic = true;
			return nullptr;
		}

		// <expression>
		auto rhs = parse_expression(context);
		if (!rhs) {
			context.had_panic = true;
			return nullptr;
		}

		// ;
		if (const auto result = require(context, TokenType::Semicolon); !result) {
			context.had_panic = true;
			return nullptr;
		}

		return AssignNode::create(std::move(lhs), std::move(rhs));
	}

	ASTNode::Dependency Parser::parse_binary(Context& context)
	{
		// <binary_expression> ::= <expression> <binary_operator> <expression>

		// <expression>
		auto lhs = parse_expression(context);
		if (!lhs) {
			return nullptr;
		}

		// <binary_operator>
		static constexpr auto k_binary_operators = {
			// Comparison operators
			TokenType::Greater,
			TokenType::GreaterEqual,
			TokenType::Less,
			TokenType::LessEqual,
			TokenType::DoubleEqual,

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
		auto binary_operator = require_any(context, k_binary_operators);
		if (!binary_operator) {
			return nullptr;
		}

		// <expression>
		auto rhs = parse_expression(context);
		if (!rhs) {
			return nullptr;
		}

		return BinaryNode::create(std::move(lhs), std::move(rhs), to_node_operator(binary_operator->type()));
	}

	ASTNode::Dependency Parser::parse_for_loop(Context& context)
	{
		// <for_loop> ::= <for> ( [<expression>] ; [<expression>] ; [<expression>] ) <block_statement>

		// <for>
		if (const auto result = require(context, TokenType::KeywordFor); !result) {
			return nullptr;
		}

		// (
		if (const auto result = require(context, TokenType::ParenLeft); !result) {
			return nullptr;
		}
		if (const auto result = match(context, TokenType::ParenRight); result) {
			// Error! Empty parentheses, expression is needed.
			return nullptr;
		}

		// [Optional] <expression>
		auto initialization = parse_expression(context);

		// ;
		if (const auto result = match(context, TokenType::Semicolon); !result) {
			return nullptr;
		}

		// [Optional] <expression>
		auto condition = parse_expression(context);

		// ;
		if (const auto result = match(context, TokenType::Semicolon); !result) {
			return nullptr;
		}

		// [Optional] <expression>
		auto update = parse_expression(context);

		// )
		if (const auto result = require(context, TokenType::ParenRight); !result) {
			return nullptr;
		}

		// <block_statement>
		auto statements = parse_block_statement(context);

		return LoopNode::create(std::move(initialization), std::move(condition), std::move(update), std::move(statements));
	}

	ASTNode::Dependency Parser::parse_foreach_loop(Context& context)
	{
		// <foreach_loop> ::=  <foreach> ( <short_variable_declaration> <in> <expression> ) <block_statement>

		// <foreach>
		if (const auto result = require(context, TokenType::KeywordForeach); !result) {
			return nullptr;
		}

		// (
		if (const auto result = require(context, TokenType::ParenLeft); !result) {
			return nullptr;
		}
		if (const auto result = match(context, TokenType::ParenLeft); result) {
			// Error! Empty parentheses, expression is needed.
			return nullptr;
		}

		// <short_variable_declaration>

		// <in>

		// <expression>

		// <block_statement>
		auto statements = parse_block_statement(context);

		return LoopNode::create(std::move(), std::move(), std::move(), std::move(statements));
	}

	ASTNode::Dependency Parser::parse_function_declaration(Context& context)
	{
		// <function_declaration> ::= <fn> <name_identifier> [<parameter_list>] :: <type_identifier> <block_statement>

		// <fn>
		if (const auto result = require(context, TokenType::KeywordFn); !result) {
			return nullptr;
		}

		// <name_identifier>
		const auto name_identifier = require(context, TokenType::LiteralIdentifier);
		if (!name_identifier) {
			return nullptr;
		}

		// [Optional] <parameter_list>
		ASTNode::Dependencies parameters;
		if (const auto has_parenthesis = match(context, TokenType::ParenLeft); has_parenthesis) {
			// <parameter_list> ::= ( [<short_variable_declaration> [, ...]] )

			// If the next token is not a closing parenthesis we assume that the list of parameters is not empty.
			if (const auto has_parameters = match(context, TokenType::ParenRight); has_parameters) {
				for (;;) {
					if (const auto result = match(context, TokenType::EndOfFile); result) {
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
					if (const auto result = match_any(context, k_continuation_types); !result) {
						// Parameter list was exhausted.
						break;
					}
				}
			}
		}

		// ::
		if (const auto result = require(context, TokenType::DoubleColon); !result) {
			return nullptr;
		}

		// <type_identifier>
		const auto type_identifier = require(context, TokenType::LiteralIdentifier);
		if (!type_identifier) {
			return nullptr;
		}

		// <block_statement>
		auto statements = parse_block_statement(context);

		return FunctionDeclarationNode::create(name_identifier->get<std::string>(),
		                                       type_identifier->get<std::string>(),
		                                       std::move(parameters),
		                                       std::move(statements));
	}

	ASTNode::Dependency Parser::parse_literal(Context& context)
	{
		// <literal> ::= <integer_literal> | <float_literal> | <string_literal>

		static constexpr auto k_literal_types
			= { TokenType::LiteralInteger, TokenType::LiteralFloat, TokenType::LiteralString };

		auto value_token = require_any(context, k_literal_types);
		if (!value_token) {
			return nullptr;
		}

		return LiteralNode::create(std::move(value_token->value()));
	}

	ASTNode::Dependency Parser::parse_struct_declaration(Context& context)
	{
		// <struct_declaration> ::= <struct> <name_identifier> { <short_variable_declaration> [, ...] }

		// <struct>
		if (const auto result = require(context, TokenType::KeywordStruct); !result) {
			return nullptr;
		}

		// <name_identifier>
		const auto name_identifier = require(context, TokenType::LiteralIdentifier);
		if (!name_identifier) {
			return nullptr;
		}

		// {
		if (const auto result = require(context, TokenType::BraceLeft); !result) {
			return nullptr;
		}

		ASTNode::Dependencies statements;
		for (;;) {
			if (const auto result = match(context, TokenType::EndOfFile); result) {
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
			if (const auto result = match_any(context, k_continuation_types); !result) {
				// Parameter list was exhausted.
				break;
			}
		}

		// }
		if (const auto result = require(context, TokenType::BraceRight); !result) {
			return nullptr;
		}

		return StructDeclarationNode::create(name_identifier->get<std::string>(), std::move(statements));
	}

	ASTNode::Dependency Parser::parse_variable_declaration(Context& context,
	                                                       bool     require_keyword,
	                                                       bool     require_expression,
	                                                       bool     require_semicolon)
	{
		// <variable_declaration> ::= <let> [<mut>] <name_identifier> : <type_identifier> = <expression>;

		// <let>
		if (const auto result = require(context, TokenType::KeywordLet); require_keyword && !result) {
			return nullptr;
		}

		// [<mut>]
		const bool is_mutable = require_keyword && match(context, TokenType::KeywordMut);

		// <name_identifier>
		const auto name_identifier = require(context, TokenType::LiteralIdentifier);
		if (!name_identifier) {
			return nullptr;
		}

		// :
		if (const auto result = require(context, TokenType::Colon); !result) {
			return nullptr;
		}

		// <type_identifier>
		const auto type_identifier = require(context, TokenType::LiteralIdentifier);
		if (!type_identifier) {
			return nullptr;
		}

		ASTNode::Dependency expression = nullptr;
		if (require_expression) {
			// =
			if (const auto result = require(context, TokenType::Equal); !result) {
				return nullptr;
			}

			// <expression>
			expression = parse_expression(context);
			if (!expression) {
				return nullptr;
			}
		}

		// ;
		if (const auto result = match(context, TokenType::Semicolon); require_semicolon && !result) {
			return nullptr;
		}

		return VariableDeclarationNode::create(name_identifier->get<std::string>(),
		                                       type_identifier->get<std::string>(),
		                                       std::move(expression),
		                                       is_mutable);
	}

	ASTNode::Dependency Parser::parse_while_loop(Context& context)
	{
		// <while_loop_statement> ::=  while ( <expression> ) <block_statement>

		// <while>
		if (const auto result = require(context, TokenType::KeywordWhile); !result) {
			return nullptr;
		}

		// (
		if (const auto result = require(context, TokenType::ParenLeft); !result) {
			return nullptr;
		}

		if (const auto result = match(context, TokenType::ParenRight); result) {
			// Error! Empty parentheses, expression is needed.
			return nullptr;
		}

		// <expression>
		auto condition = parse_expression(context);
		if (!condition) {
			return nullptr;
		}

		// )
		if (const auto result = require(context, TokenType::ParenRight); !result) {
			return nullptr;
		}

		// <block_statement>
		auto statements = parse_block_statement(context);

		return LoopNode::create(nullptr, std::move(condition), nullptr, std::move(statements));
	}

	ASTNode::Dependencies Parser::parse_block_statement(Context& context) {}

	std::optional<Token> Parser::require(Context& context, TokenType type)
	{
		if (context.tokens[context.current_index].type() == type) {
			return context.tokens[context.current_index++];
		}
		return std::nullopt;
	}

	std::optional<Token> Parser::require_any(Context& context, std::span<const TokenType> types)
	{
		for (const auto& type : types) {
			if (const auto result = require(context, type); result) {
				return result;
			}
		}
		return std::nullopt;
	}

	bool Parser::match(Context& context, TokenType type)
	{
		if (context.tokens[context.current_index].type() == type) {
			context.current_index++;
			return true;
		}
		return false;
	}

	bool Parser::match_any(Context& context, std::span<const TokenType> types)
	{
		for (const auto& type : types) {
			if (const auto result = match(context, type); result) {
				return result;
			}
		}
		return false;
	}

	void Parser::synchronize(Context& context)
	{
		static constexpr auto k_synchronization_tokens = {
			TokenType::KeywordFn,     TokenType::KeywordLet,   TokenType::KeywordIf,
			TokenType::KeywordFor,    TokenType::KeywordWhile, TokenType::KeywordReturn,
			TokenType::KeywordStruct, TokenType::BraceLeft /* Scope */
		};

		while (!match(context, TokenType::EndOfFile)) {
			if (match_any(context, k_synchronization_tokens)) {
				break;  // Synchronized.
			}
			context.current_index++;
		}
		context.had_panic = false;
	}
}  // namespace soul
