#pragma once

#include "ast/ast.h"
#include "common/diagnostic.h"
#include "lexer/token.h"

#include <format>
#include <optional>
#include <span>

namespace soul
{
	/**
	 * @brief Class used for performing parsing an input stream of tokens
	 * into an Abstract Syntax Tree (AST).
	 */
	class Parser
	{
		public:
		struct Context
		{
			std::span<const Token> tokens        = {};
			std::size_t      current_index = 0;
			bool             had_panic     = false;
			bool             had_error     = false;
			Diagnostics      diagnostics;
		};

		public:
		/**
		 * @brief Converts linear sequence of tokens into an Abstract Syntax Tree (AST).
		 * @param tokens tokens to be parsed.
		 * @return AST if the parsing was a success, nullptr otherwise.
		 */
		ASTNode::Dependency parse(std::span<const Token> tokens);

		private:
		ASTNode::Dependency parse_statement(Context& context);
		ASTNode::Dependency parse_expression(Context& context);

		ASTNode::Dependency parse_assign(Context& context);
		ASTNode::Dependency parse_binary(Context& context);
		ASTNode::Dependency parse_for_loop(Context& context);
		ASTNode::Dependency parse_foreach_loop(Context& context);
		ASTNode::Dependency parse_function_declaration(Context& context);
		ASTNode::Dependency parse_literal(Context& context);
		ASTNode::Dependency parse_struct_declaration(Context& context);
		ASTNode::Dependency parse_variable_declaration(Context& context,
		                                               bool     require_keyword    = true,
		                                               bool     require_expression = true,
		                                               bool     require_semicolon  = true);
		ASTNode::Dependency parse_while_loop(Context& context);

		ASTNode::Dependencies parse_block_statement(Context& context);

		/**
		 * @brief Verifies that the current token is of a given type.
		 * @return If it does match then the token is consumed and returned; false otherwise.
		 */
		std::optional<Token> require(Context& context, TokenType type);
		std::optional<Token> require_any(Context& context, std::span<const TokenType> types);

		/**
		 * @brief Verifies that the current token is of a given type.
		 * @return If it does match then true is returned and the token is consumed; false otherwise.
		 */
		bool match(Context& context, TokenType type);
		bool match_any(Context& context, std::span<const TokenType> types);

		void synchronize(Context& context);
	};

}  // namespace soul
