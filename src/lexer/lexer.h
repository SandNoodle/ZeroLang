#pragma once

#include "common/diagnostic.h"
#include "lexer/token.h"

#include <functional>
#include <optional>
#include <string_view>
#include <vector>

namespace soul::lexer
{
	/**
	 * @brief Class used performing a lexical analysis on the given input text.
	 */
	class Lexer
	{
		public:
		using Char = char;

		private:
		std::string_view _script         = {};
		size_t           _start_index    = 0;
		size_t           _current_offset = 0;
		Diagnostics*     _diagnostics    = nullptr;

		public:
		/**
		 * @brief Converts the input script into a linear sequence of tokens
		 * in a process known as a 'tokenization'.
		 * @param script Script that will be parsed / tokenized.
		 * @param Diagnostic
		 * @return Tokenized sequence of a given script.
		 */
		[[nodiscard]] static std::vector<Token> tokenize(std::string_view script, Diagnostics* diagnostics = nullptr);

		private:
		Lexer(std::string_view script, Diagnostics* diagnostics = nullptr);

		std::vector<Token>   tokenize();
		std::optional<Token> scan_token();
		Token                create_identifier_token();
		std::optional<Token> create_numeric_token();
		std::optional<Token> create_string_token();

		/**
		 * @brief Creates diagnostic message.
		 * @param code Diagnostic code - determines the message.
		 * @param args Arguments to format the diagnostic string with.
		 */
		template <typename... Args>
		void diagnostic(DiagnosticType type, DiagnosticCode code, Args&&... args) const
		{
			if (!_diagnostics) {
				return;
			}
			_diagnostics->emplace_back(type, code, std::forward<Args>(args)...);
		}

		void             skip_whitespace();
		std::string_view current_token() const;
		Char             peek(size_t count = 0) const;
		Char             advance();
		bool             is_eof(Lexer::Char c);
		bool             is_whitespace(Lexer::Char c);
		bool             is_alpha(Lexer::Char c);
		bool             is_digit(Lexer::Char c);
		bool             is_hex_digit(Lexer::Char c);
		bool             is_quote(Lexer::Char c);
	};
}  // namespace soul::lexer
