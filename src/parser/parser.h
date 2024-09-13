#pragma once

#include "ast/ast.h"
#include "lexer/token.h"
#include "common/diagnostic.h"

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
		private:
			std::span<Token> _tokens;
			size_t _current_index = 0;
			bool _had_panic = false;
			bool _had_error = false;
			mutable std::vector<Diagnostic> _diagnostics;

		public:
			/**
			 * @brief Converts linear sequence of tokens into an Abstract Syntax Tree (AST).
			 * @param tokens tokens to be parsed.
			 * @return AST if the parsing was a success, nullptr otherwise.
			 */
			ASTNode::dependency_t parse(std::span<Token> tokens);

			/**
			 * @brief Returns all diagnostic messages collected during parsing.
			 * @detail If not diagnostic messages were collected, the vector is empty.
			 */
			[[nodiscard]] const std::vector<Diagnostic>& diagnostics() const noexcept
			{
				return _diagnostics;
			}

		private:
			ASTNode::dependency_t parse_statement();
			ASTNode::dependency_t parse_expression();

			ASTNode::dependency_t parse_variable_declaration();
			ASTNode::dependency_t parse_function_declaration();
			ASTNode::dependencies_t parse_block_statement();

			/**
			 * @Brief Returns the type of the current token.
			 * @return Type of the current token, or EOF otherwise.
			 */
			TokenType current_token_type();

			/**
			 * @brief
			 */
			void synchronize();

			/**
			 * @brief Advances the parser to the next token.
			 * @detail If there are no more tokens left - its an noop and
			 * a diagnostic error is emitted.
			 */
			void advance();

			/**
			 * @brief Creates diagnostic message.
			 * @param code Diagnostic code - determines the message.
			 * @param args Arguments to format the diagnostic string with.
			 */
			template <typename... Args>
			void diagnostic(DiagnosticCode code, Args&&... args) const
			{
				_diagnostics.emplace_back(code, std::forward<Args>(args)...);
			}

			/**
			 * @brief Verifies that the next token has to be of a given type.
             * @return If it is, then that token is returned and advance() is called.
			 * If not, std::nullopt is returned and the _has_error is set.
			 */
			[[nodiscard]] std::optional<Token> require(TokenType type);


            /**
             * @brief Verifies that the next token is of a given type.
             * @detail If the token is matched, then advance() is called.
             * @return Returns true if it is, false otherwise.
             */
            [[nodiscard]] bool match(TokenType type);

            /**
             * @brief Verifies that the next token is one of a given types.
             * @detail If the token is matched, then advance() is called.
             * @return Returns true if it is, false otherwise.
             */
            [[nodiscard]] bool match_any(std::span<TokenType> types);

			[[nodiscard]] static bool is_synchronization_token(TokenType token);
	};
}
