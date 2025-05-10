#pragma once

#include "ast/ast.h"
#include "common/diagnostic.h"
#include "lexer/token.h"

#include <format>
#include <functional>
#include <optional>
#include <span>

namespace soul::parser
{
	/**
	 * @brief Class implementing parser for Soul language.
	 * Translates list of tokens into an Abstract Syntax Tree (AST).
	 */
	class Parser
	{
		public:
		struct PrecedenceRule;
		enum class Precedence : u8;

		private:
		std::span<const Token> _tokens        = {};
		std::size_t            _current_index = 0;
		bool                   _had_panic     = false;
		bool                   _had_error     = false;

		public:
		/**
		 * @brief Converts linear sequence of tokens into an Abstract Syntax Tree (AST).
		 * @param tokens tokens to be parsed.
		 * @return AST if the parsing was a success, nullptr otherwise.
		 */
		[[nodiscard]] static ast::ASTNode::Dependency parse(std::span<const Token> tokens);

		private:
		Parser(std::span<const Token> tokens);
		ast::ASTNode::Dependency parse();
		ast::ASTNode::Dependency parse_expression();
		ast::ASTNode::Dependency parse_expression_statement();
		ast::ASTNode::Dependency parse_expression_with_precedence(Precedence precedence);
		ast::ASTNode::Dependency parse_statement();

		ast::ASTNode::Dependency parse_assign();
		ast::ASTNode::Dependency parse_cast();
		ast::ASTNode::Dependency parse_binary(ast::ASTNode::Dependency lhs);
		ast::ASTNode::Dependency parse_for_loop();
		ast::ASTNode::Dependency parse_foreach_loop();
		ast::ASTNode::Dependency parse_function_declaration();
		ast::ASTNode::Dependency parse_if();
		ast::ASTNode::Dependency parse_literal();
		ast::ASTNode::Dependency parse_struct_declaration();
		ast::ASTNode::Dependency parse_suffix();
		ast::ASTNode::Dependency parse_unary();
		ast::ASTNode::Dependency parse_variable_declaration(bool require_keyword    = true,
		                                                    bool require_expression = true,
		                                                    bool require_semicolon  = true);
		ast::ASTNode::Dependency parse_while_loop();

		ast::ASTNode::Dependencies parse_block_statement();

		PrecedenceRule get_precedence_rule(TokenType type) const noexcept;

		/**
		 * @brief Verifies that the current token is of a given type.
		 * @return If it does match then the token is consumed and returned; false otherwise.
		 */
		std::optional<Token> require(TokenType type);
		std::optional<Token> require_any(std::span<const TokenType> types);

		/**
		 * @brief Verifies that the current token is of a given type.
		 * @return If it does match then true is returned and the token is consumed; false otherwise.
		 */
		bool try_match(TokenType type);
		bool try_match_any(std::span<const TokenType> types);

		/** @brief Advances the parser forwards and returns the (now) previous token. */
		const Token& advance() noexcept;

		/**
		 * @brief Tries to advance the parser up until synchronization token is encountered, removing panic in the
		 * process.
		 */
		void synchronize();

		const Token* previous_token() const noexcept;
		const Token* current_token() const noexcept;
		const Token* next_token() const noexcept;
	};
}  // namespace soul::parser
