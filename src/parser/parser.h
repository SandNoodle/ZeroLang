#pragma once

#include "ast/nodes/error.h"
#include "ast/nodes/nodes_fwd.h"
#include "core/types.h"
#include "lexer/token.h"

#include <span>
#include <string_view>

namespace soul::parser
{
	/**
	 * @brief Parser performs syntactic analysis on a given linear list of lexical tokens, i.e. converts it into an
	 * Abstract Syntax Tree (AST).
	 */
	class Parser
	{
		private:
		struct PrecedenceRule;
		enum class Precedence : u8;

		private:
		std::span<const Token>           _tokens        = {};
		std::span<const Token>::iterator _current_token = {};
		std::string_view                 _module_name   = {};

		public:
		/**
		 * @brief Converts linear sequence of tokens into an Abstract Syntax Tree (AST).
		 * @param module_name Name of the module.
		 * @param tokens Tokens to be parsed.
		 * @return Module with parsed statements.
		 */
		[[nodiscard]] static ast::ASTNode::Dependency parse(std::string_view       module_name,
		                                                    std::span<const Token> tokens);

		private:
		Parser(std::string_view module_name, std::span<const Token> tokens);

		ast::ASTNode::Dependency parse();
		ast::ASTNode::Dependency parse_statement();
		ast::ASTNode::Dependency parse_expression();
		ast::ASTNode::Dependency parse_expression(Precedence precedence);

		ast::ASTNode::Dependency parse_binary(ast::ASTNode::Dependency lhs);
		ast::ASTNode::Dependency parse_cast();
		ast::ASTNode::Dependency parse_for_loop();
		ast::ASTNode::Dependency parse_function_call(ast::ASTNode::Dependency dependency);
		ast::ASTNode::Dependency parse_function_declaration();
		ast::ASTNode::Dependency parse_if();
		ast::ASTNode::Dependency parse_literal();
		ast::ASTNode::Dependency parse_struct_declaration();
		ast::ASTNode::Dependency parse_unary();
		ast::ASTNode::Dependency parse_variable_declaration();
		ast::ASTNode::Dependency parse_while_loop();

		ast::ASTNode::Dependencies parse_block_statement();
		ast::ASTNode::Dependency   parse_parameter_declaration();

		/**
		 * @brief Creates new Error node in the AST and resynchronizes the parser.
		 */
		ast::ASTNode::Dependency create_error(ast::nodes::ErrorNode::Message error_message);

		std::optional<Token> require(Token::Type type);
		std::optional<Token> require(std::span<const Token::Type> types);
		std::optional<Token> peek(std::ptrdiff_t n);
		bool                 match(Token::Type type);

		PrecedenceRule precedence_rule(Token::Type type) const noexcept;

		/** @brief Returns current token or an explicit EOF one. */
		Token current_token_or_default() const noexcept;
	};
}  // namespace soul::parser
