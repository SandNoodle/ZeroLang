#pragma once

#include "lexer/token.h"

#include <string_view>
#include <vector>

namespace soul
{
	class lexer_t
	{
		public:
			using char_t = char;

		private:
			std::string_view _script;
			size_t _start_index    = 0;
			size_t _current_offset = 0;

		public:
			/**
			 * @brief Converts the input script into a linear sequence of tokens
			 * in a process known as a 'tokenization'.
			 * @param script Script that will be parsed / tokenized.
			 * @return Tokenized sequence of a given script.
			 */
			[[nodiscard]] std::vector<token_t> tokenize(std::string_view script);

		private:
			token_t scan_token();
			token_t create_identifier_token();
			token_t create_numeric_token();
			token_t create_string_token();
			token_t create_error_token(std::string_view message);

			void skip_whitespace();
			char_t peek(size_t count = 0) const;
			char_t advance();

			std::string_view current_token() const;
	};
}  // namespace soul
