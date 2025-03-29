#pragma once

#include "common/diagnostic.h"
#include "lexer/token.h"

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
		std::string_view    _script;
		size_t              _start_index    = 0;
		size_t              _current_offset = 0;
		mutable Diagnostics _diagnostics;

		public:
		/**
		 * @brief Converts the input script into a linear sequence of tokens
		 * in a process known as a 'tokenization'.
		 * @param script Script that will be parsed / tokenized.
		 * @return Tokenized sequence of a given script.
		 */
		[[nodiscard]] std::vector<Token> tokenize(std::string_view script);

		/**
		 * @brief Returns all diagnostic messages collected during lexing.
		 * @detail If not diagnostic messages were collected, the vector is empty.
		 */
		[[nodiscard]] const Diagnostics& diagnostics() const noexcept { return _diagnostics; }

		private:
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
			_diagnostics.emplace_back(type, code, std::forward<Args>(args)...);
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
