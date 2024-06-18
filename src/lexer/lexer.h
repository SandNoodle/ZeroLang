#pragma once

#include "lexer/token.h"
#include "common/diagnostic.h"

#include <optional>
#include <string_view>
#include <vector>

namespace soul
{
	/**
	 * @brief Class used performing a lexical analysis on the given input text.
	 */
	class lexer_t
	{
		public:
			using char_t = char;

		private:
			std::string_view _script;
			size_t _start_index    = 0;
			size_t _current_offset = 0;
			mutable std::vector<diagnostic_t> _diagnostics;

		public:
			/**
			 * @brief Converts the input script into a linear sequence of tokens
			 * in a process known as a 'tokenization'.
			 * @param script Script that will be parsed / tokenized.
			 * @return Tokenized sequence of a given script.
			 */
			[[nodiscard]] std::vector<token_t> tokenize(std::string_view script);

			/**
			 * @brief Returns all diagnostic messages collected during lexing.
			 * @detail If not diagnostic messages were collected, the vector is empty.
			 */
			[[nodiscard]] const std::vector<diagnostic_t>& diagnostics() const noexcept
			{
				return _diagnostics;
			}

		private:
			std::optional<token_t> scan_token();
			token_t create_identifier_token();
			std::optional<token_t> create_numeric_token();
			std::optional<token_t> create_string_token();

			/**
			 * @brief Creates diagnostic message.
			 * @param code Diagnostic code - determines the message.
			 * @param args Arguments to format the diagnostic string with.
			 */
			template <typename... Args>
			void diagnostic(diagnostic_code_t code, Args&&... args) const
			{
				_diagnostics.emplace_back(code, std::forward<Args>(args)...);
			}

			void skip_whitespace();
			char_t peek(size_t count = 0) const;
			char_t advance();

			std::string_view current_token() const;
	};
}  // namespace soul
