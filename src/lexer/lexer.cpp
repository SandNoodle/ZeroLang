#include "lexer/lexer.h"

#include <charconv>
#include <unordered_map>

namespace soul
{
	static bool is_eof(lexer_t::char_t c);
	static bool is_whitespace(lexer_t::char_t c);
	static bool is_alpha(lexer_t::char_t c);
	static bool is_digit(lexer_t::char_t c);
	static bool is_hex_digit(lexer_t::char_t c);
	static bool is_quote(lexer_t::char_t c);

	[[nodiscard]] std::vector<token_t> lexer_t::tokenize(std::string_view script)
	{
		std::vector<token_t> result;
		if (script.empty())
		{
			result.emplace_back(token_type_t::token_eof);
			return result;
		}

		_current_offset = 0;
		_start_index = 0;
		_script = script;
		for(;;)
		{
			result.emplace_back(scan_token());
			if (result.back().type() == token_type_t::token_eof)
				break;
		}
		return result;
	}

	token_t lexer_t::scan_token()
	{
		skip_whitespace();

		_start_index = _current_offset;
		char_t c = peek();

		if (is_eof(c))
			return token_t(token_type_t::token_eof);

		if (is_alpha(c))
			return create_identifier_token();

		if (is_digit(c) || (c == '-' && is_digit(peek(1))))
			return create_numeric_token();

		if (is_quote(c))
			return create_string_token();

		// Special characters
		static const std::unordered_map<std::string_view, token_type_t> k_tokens = {
			// Single character tokens
			{ ";", token_type_t::token_semicolon },    { "?", token_type_t::token_question_mark },
			{ "%", token_type_t::token_percent },      { "^", token_type_t::token_caret },
			{ ".", token_type_t::token_dot },          { ",", token_type_t::token_comma },
			{ "(", token_type_t::token_paren_left },   { ")", token_type_t::token_paren_right },
			{ "{", token_type_t::token_brace_left },   { "}", token_type_t::token_brace_right },
			{ "[", token_type_t::token_bracket_left }, { "]", token_type_t::token_bracket_right },

			// One or two character tokens
			{ ":", token_type_t::token_colon },     { "::", token_type_t::token_double_colon },
			{ "=", token_type_t::token_equal },     { "==", token_type_t::token_double_equal },
			{ "!", token_type_t::token_bang },      { "!=", token_type_t::token_bang_equal },
			{ ">", token_type_t::token_greater },   { ">=", token_type_t::token_greater_equal },
			{ "<", token_type_t::token_less },      { "<=", token_type_t::token_less_equal },
			{ "+", token_type_t::token_plus },      { "+=", token_type_t::token_plus_equal },  { "++", token_type_t::token_double_plus },
			{ "-", token_type_t::token_minus },     { "-=", token_type_t::token_minus_equal }, { "--", token_type_t::token_double_minus },
			{ "*", token_type_t::token_star },      { "*=", token_type_t::token_star_equal },
			{ "/", token_type_t::token_slash },     { "/=", token_type_t::token_slash_equal },
			{ "&", token_type_t::token_ampersand }, { "&&", token_type_t::token_double_ampersand },
			{ "|", token_type_t::token_pipe },      { "||", token_type_t::token_double_pipe },
		};

		c = advance();

		// At maximum special tokens are a combination of two characters.
		if (!is_eof(c) && !is_whitespace(c))
			_current_offset++;
		
		const auto current_token = this->current_token();
		if (k_tokens.contains(current_token))
			return token_t(k_tokens.at(current_token));

		return create_error_token("unrecognized token");
	}

	token_t lexer_t::create_identifier_token()
	{
		while (is_alpha(peek()))
			advance();

		// Keywords
		static const std::unordered_map<std::string_view, token_type_t> k_keywords = {
			// Keywords
			{ "break",    token_type_t::token_break },
			{ "continue", token_type_t::token_continue },
			{ "else",     token_type_t::token_else },
			{ "false",    token_type_t::token_false },
			{ "fn",       token_type_t::token_fn },
			{ "for",      token_type_t::token_for },
			{ "if",       token_type_t::token_if },
			{ "let",      token_type_t::token_let },
			{ "mut",      token_type_t::token_mut },
			{ "native",   token_type_t::token_native },
			{ "return",   token_type_t::token_return },
			{ "struct",   token_type_t::token_struct },
			{ "true",     token_type_t::token_true },
			{ "while",    token_type_t::token_while },
		};

		const auto current_token = this->current_token();
		if (k_keywords.contains(current_token))
			return token_t(k_keywords.at(current_token));

		// Identifier
		return token_t(token_type_t::token_literal_identifier, std::string(current_token));
	}

	// TODO(sand_noodles):
	//   This method handles only Integers and Floats.
	//   Add support for scanning:
	//   - binary (ex. 0x0111b)
	//   - hex (ex. 0xFF)
	token_t lexer_t::create_numeric_token()
	{
		if (peek() == '-')
			advance();

		while(is_hex_digit(peek()) || peek() == '.' || peek() == 'x')
			advance();

		const auto current_token = this->current_token();

		const auto create_token = [this] <is_value_t T> (std::string_view token, token_type_t type) -> token_t {
			T value = 0;
			const auto [_, error_code] = std::from_chars(token.data(), token.data() + token.size(), value);

			if (error_code == std::errc{}) [[likely]]
				return token_t(type, value); // No error.
			else if (error_code == std::errc::invalid_argument)
				return create_error_token("value is not a number");
			else if (error_code == std::errc::result_out_of_range)
				return create_error_token("value out of range");

			return create_error_token("unrecognized");
		};

		return current_token.find('.') != std::string_view::npos ?
			create_token.operator()<double>(current_token, token_type_t::token_literal_float) :
			create_token.operator()<int64_t>(current_token, token_type_t::token_literal_integer);
	}

	token_t lexer_t::create_string_token()
	{
		char_t c = advance(); // Skip "
		while(!is_quote(c) && !is_eof(c))
		{
			c = advance();
		}

		if (is_eof(c))
			return create_error_token("unterminated string");

		const auto current_token = _script.substr(_start_index + 1, _current_offset - _start_index - 1);
		advance();
		return token_t(token_type_t::token_literal_string, std::string(current_token));
	}

	token_t lexer_t::create_error_token(std::string_view message)
	{
		return token_t(token_type_t::token_error, std::string(message));
	}

	void lexer_t::skip_whitespace()
	{
		for(;;)
		{
			char_t c = peek();
			switch(c)
			{
				case ' ':
				case '\r':
				case '\t':
				case '\n':
					advance();
					break;
				case '#':
					advance(); // Skip the '#'
					c = peek();
					while (!is_eof(c) && (c != '\n' || c != '\r'))
					{
						advance(); // Skip the '#'
						c = peek();
					}
					break;
				default:
					return;
			}
		}
	}

	lexer_t::char_t lexer_t::peek(size_t count) const
	{
		if (_current_offset + count >= _script.size())
			return '\0'; // TODO: Error?
		return _script[_current_offset + count];
	}

	lexer_t::char_t lexer_t::advance()
	{
		_current_offset++;
		return peek();
	}

	std::string_view lexer_t::current_token() const
	{
		return _script.substr(_start_index, _current_offset - _start_index);
	}

	//
	//
	//

	static bool is_eof(lexer_t::char_t c)
	{
		return c == '\0';
	}

	static bool is_whitespace(lexer_t::char_t c)
	{
		return c == ' ' || c == '\t' || c == '\n' || c == '\r';
	}

	static bool is_alpha(lexer_t::char_t c)
	{
		return (c >= 'a' && c <= 'z') ||  (c >= 'A' && c <= 'Z') || c == '_';
	}

	static bool is_digit(lexer_t::char_t c)
	{
		return c >= '0' && c <= '9';
	}

	static bool is_hex_digit(lexer_t::char_t c)
	{
		return is_digit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
	}

	static bool is_quote(lexer_t::char_t c)
	{
		return c == '"' || c == '\'';
	}
}  // namespace soul
