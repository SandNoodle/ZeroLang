#pragma once

#include "ast/ast.h"
#include "common/diagnostic.h"
#include "lexer/token.h"

#include <format>
#include <functional>
#include <optional>
#include <span>

namespace soul
{
	/**
	 * @brief Class implementing parser for Soul language.
	 * Translates list of tokens into an Abstract Syntax Tree (AST).
	 */
	class Parser
	{
		public:
		class Context;
		struct PrecedenceRule;
		enum class Precedence : u8;

		public:
		/**
		 * @brief Converts linear sequence of tokens into an Abstract Syntax Tree (AST).
		 * @param tokens tokens to be parsed.
		 * @return AST if the parsing was a success, nullptr otherwise.
		 */
		ASTNode::Dependency parse(std::span<const Token> tokens);

		private:
		ASTNode::Dependency parse_expression(Context& context);
		ASTNode::Dependency parse_expression_statement(Context& context);
		ASTNode::Dependency parse_expression_with_precedence(Context& context, Precedence precedence);
		ASTNode::Dependency parse_statement(Context& context);

		ASTNode::Dependency parse_assign(Context& context);
		ASTNode::Dependency parse_binary(Context& context, ASTNode::Dependency lhs);
		ASTNode::Dependency parse_for_loop(Context& context);
		ASTNode::Dependency parse_foreach_loop(Context& context);
		ASTNode::Dependency parse_function_declaration(Context& context);
		ASTNode::Dependency parse_if(Context& context);
		ASTNode::Dependency parse_literal(Context& context);
		ASTNode::Dependency parse_struct_declaration(Context& context);
		ASTNode::Dependency parse_suffix(Context& context);
		ASTNode::Dependency parse_unary(Context& context);
		ASTNode::Dependency parse_variable_declaration(Context& context,
		                                               bool     require_keyword    = true,
		                                               bool     require_expression = true,
		                                               bool     require_semicolon  = true);
		ASTNode::Dependency parse_while_loop(Context& context);

		ASTNode::Dependencies parse_block_statement(Context& context);

		PrecedenceRule get_precedence_rule(TokenType type) const noexcept;
	};

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

	class Parser::Context
	{
		private:
		std::span<const Token> _tokens{};
		std::size_t            _current_index = 0;

		public:
		bool had_panic = false;
		bool had_error = false;

		public:
		Context(std::span<const Token> tokens);

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
}  // namespace soul
