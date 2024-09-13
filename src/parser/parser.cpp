#include "parser/parser.h"

#include "ast/nodes/binary.h"
#include "ast/nodes/function_declaration.h"
#include "ast/nodes/unary.h"
#include "ast/nodes/variable_declaration.h"

#include <algorithm>
#include <vector>

namespace soul
{
#define HANDLE_REQUIRE(condition) \
	if (!(condition)) return nullptr;

	ASTNode::dependency_t Parser::parse(std::span<Token> tokens)
	{
		_tokens        = tokens;
		_current_index = 0;
		_had_panic     = false;
		_had_error     = false;
		_diagnostics.clear();

		std::vector<ASTNode::dependency_t> statements;
		while (current_token_type() != TokenType::TOKEN_EOF) {
			statements.emplace_back(parse_statement());
			if (_had_panic) synchronize();
		}
		return nullptr;  // TODO
	}

	ASTNode::dependency_t Parser::parse_statement()
	{
		const auto type = current_token_type();
		switch (type) {
			case TokenType::TOKEN_LET:
				return parse_variable_declaration();
			case TokenType::TOKEN_FN:
				return parse_function_declaration();
			default:
				break;
		}

		// TODO
		return nullptr;
	}

	ASTNode::dependency_t Parser::parse_expression() { return nullptr; }

	// VARIABLE_DECLARATION ::= "let" ["mut"] <identifier> : <type_specifier> = <expression> ";"
	ASTNode::dependency_t Parser::parse_variable_declaration()
	{
		// Keyword
		HANDLE_REQUIRE(require(TokenType::TOKEN_LET));

		// (Optional) Mutability
		const bool is_mutable = match(TokenType::TOKEN_MUT);

		// Identifier
		const auto identifier_token = require(TokenType::TOKEN_LITERAL_IDENTIFIER);
		HANDLE_REQUIRE(identifier_token);
		auto identifier = (*identifier_token).get<std::string>();

		// Type
		HANDLE_REQUIRE(require(TokenType::TOKEN_COLON));
		const auto type_token = require(TokenType::TOKEN_LITERAL_IDENTIFIER);
		HANDLE_REQUIRE(type_token);
		auto type = (*type_token).get<std::string>();

		// Expression
		auto expression = parse_expression();
		if (!expression) {
		}

		return VariableDeclarationNode::create(
			std::move(identifier), std::move(type), std::move(expression), is_mutable);
	}

	// FUNCTION_DECLARATION ::= "fn" <identifier> :: <type_specifier> <block_statement>
	ASTNode::dependency_t Parser::parse_function_declaration()
	{
		// Keyword
		HANDLE_REQUIRE(require(TokenType::TOKEN_FN));

		// Identifier
		auto identifier_token = require(TokenType::TOKEN_LITERAL_IDENTIFIER);
		HANDLE_REQUIRE(identifier_token);
		auto identifier = std::move((*identifier_token).get<std::string>());

		// (Optional) Parameters
		ASTNode::dependencies_t parameters;

		// Type
		HANDLE_REQUIRE(require(TokenType::TOKEN_DOUBLE_COLON));
		auto return_type_token = require(TokenType::TOKEN_LITERAL_IDENTIFIER);
		HANDLE_REQUIRE(return_type_token);
		auto return_type = (*return_type_token).get<std::string>();

		// Statements
		ASTNode::dependencies_t statements = parse_block_statement();

		return FunctionDeclarationNode::create(
			std::move(identifier), std::move(return_type), std::move(parameters), std::move(statements));
	}

	// BLOCK_STATEMENT ::= "{" { <statement> } "}"
	ASTNode::dependencies_t Parser::parse_block_statement()
	{
		const auto enter_block_result = require(TokenType::TOKEN_BRACE_LEFT);
		if (!enter_block_result) return {};

		ASTNode::dependencies_t statements;
		while (current_token_type() != TokenType::TOKEN_BRACE_RIGHT) {
			statements.emplace_back(parse_statement());

			// NOTE: Prevent unterminated blocks.
			if (current_token_type() == TokenType::TOKEN_EOF) [[unlikely]] {
				diagnostic(DiagnosticCode::ERROR_PARSER_OUT_OF_RANGE);
				break;
			}
		}

		const auto exit_block_result = require(TokenType::TOKEN_BRACE_RIGHT);
		if (!exit_block_result) return {};

		return statements;
	}

	TokenType Parser::current_token_type()
	{
		if (_current_index >= _tokens.size()) [[unlikely]] {
			diagnostic(DiagnosticCode::ERROR_PARSER_OUT_OF_RANGE);
			_had_error = true;
			return TokenType::TOKEN_EOF;
		}
		return _tokens[_current_index].type();
	}

	void Parser::synchronize()
	{
		while (!match(TokenType::TOKEN_EOF)) {
			const auto type = this->current_token_type();
			if (!is_synchronization_token(type)) advance();
			break;  // Synchronized.
		}
		_had_panic = false;
	}

	void Parser::advance()
	{
		if (_current_index >= _tokens.size()) {
			diagnostic(DiagnosticCode::ERROR_PARSER_OUT_OF_RANGE);
			_had_error = true;
			return;
		}
		_current_index++;
	}

	[[nodiscard]] std::optional<Token> Parser::require(TokenType type)
	{
		const auto current_type = this->current_token_type();
		if (current_type != type) {
			_had_error = true;
			return std::nullopt;
		}

		advance();
		return _tokens[_current_index - 1];
	}

	[[nodiscard]] bool Parser::match(TokenType type)
	{
		const auto current_type = this->current_token_type();
		if (current_type != type) return false;

		advance();
		return true;
	}

	[[nodiscard]] bool Parser::match_any(std::span<TokenType> types)
	{
		return std::ranges::any_of(types, [this](const auto& type) -> bool { return match(type); });
	}

	[[nodiscard]] bool Parser::is_synchronization_token(TokenType type)
	{
		switch (type) {
			case TokenType::TOKEN_FN:
				[[fallthrough]];
			case TokenType::TOKEN_LET:
				[[fallthrough]];
			case TokenType::TOKEN_IF:
				[[fallthrough]];
			case TokenType::TOKEN_FOR:
				[[fallthrough]];
			case TokenType::TOKEN_WHILE:
				[[fallthrough]];
			case TokenType::TOKEN_RETURN:
				[[fallthrough]];
			case TokenType::TOKEN_STRUCT:
				[[fallthrough]];
			case TokenType::TOKEN_BRACE_LEFT:  // Scope
				return true;
			default:
				return false;
		}
	}
}  // namespace soul
