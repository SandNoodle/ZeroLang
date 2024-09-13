#include "lexer/lexer.h"

#include <charconv>
#include <unordered_map>

namespace soul
{
	static bool is_eof(Lexer::char_t c);

	static bool is_whitespace(Lexer::char_t c);

	static bool is_alpha(Lexer::char_t c);

	static bool is_digit(Lexer::char_t c);

	static bool is_hex_digit(Lexer::char_t c);

	static bool is_quote(Lexer::char_t c);

	[[nodiscard]] std::vector<Token> Lexer::tokenize(std::string_view script)
	{
		_current_offset = 0;
		_start_index    = 0;
		_script         = script;
		_diagnostics.clear();

		if (script.empty()) {
			return { Token(TokenType::TOKEN_EOF) };
		}

		std::vector<Token> result;
		for (;;) {
			auto result_token = scan_token();
			if (!result_token.has_value() || result_token->type() == TokenType::TOKEN_EOF) break;
			result.push_back(std::move(*result_token));
		}
		result.emplace_back(TokenType::TOKEN_EOF);
		return result;
	}

	std::optional<Token> Lexer::scan_token()
	{
		skip_whitespace();

		_start_index = _current_offset;
		char_t c     = peek();

		if (is_eof(c)) return Token(TokenType::TOKEN_EOF);

		if (is_alpha(c)) return create_identifier_token();

		if (is_digit(c) || (c == '-' && is_digit(peek(1)))) return create_numeric_token();

		if (is_quote(c)) return create_string_token();

		// Special characters
		static const std::unordered_map<std::string_view, TokenType> k_tokens = {
			// Single character tokens
			{ ";",  TokenType::TOKEN_SEMICOLON        },
			{ "?",  TokenType::TOKEN_QUESTION_MARK    },
			{ "%",  TokenType::TOKEN_PERCENT          },
			{ "^",  TokenType::TOKEN_CARET            },
			{ ".",  TokenType::TOKEN_DOT              },
			{ ",",  TokenType::TOKEN_COMMA            },
			{ "(",  TokenType::TOKEN_PAREN_LEFT       },
			{ ")",  TokenType::TOKEN_PAREN_RIGHT      },
			{ "{",  TokenType::TOKEN_BRACE_LEFT       },
			{ "}",  TokenType::TOKEN_BRACE_RIGHT      },
			{ "[",  TokenType::TOKEN_BRACKET_LEFT     },
			{ "]",  TokenType::TOKEN_BRACKET_RIGHT    },

			// One or two character tokens
			{ ":",  TokenType::TOKEN_COLON            },
			{ "::", TokenType::TOKEN_DOUBLE_COLON     },
			{ "=",  TokenType::TOKEN_EQUAL            },
			{ "==", TokenType::TOKEN_DOUBLE_EQUAL     },
			{ "!",  TokenType::TOKEN_BANG             },
			{ "!=", TokenType::TOKEN_BANG_EQUAL       },
			{ ">",  TokenType::TOKEN_GREATER          },
			{ ">=", TokenType::TOKEN_GREATER_EQUAL    },
			{ "<",  TokenType::TOKEN_LESS             },
			{ "<=", TokenType::TOKEN_LESS_EQUAL       },
			{ "+",  TokenType::TOKEN_PLUS             },
			{ "+=", TokenType::TOKEN_PLUS_EQUAL       },
			{ "++", TokenType::TOKEN_DOUBLE_PLUS      },
			{ "-",  TokenType::TOKEN_MINUS            },
			{ "-=", TokenType::TOKEN_MINUS_EQUAL      },
			{ "--", TokenType::TOKEN_DOUBLE_MINUS     },
			{ "*",  TokenType::TOKEN_STAR             },
			{ "*=", TokenType::TOKEN_STAR_EQUAL       },
			{ "/",  TokenType::TOKEN_SLASH            },
			{ "/=", TokenType::TOKEN_SLASH_EQUAL      },
			{ "&",  TokenType::TOKEN_AMPERSAND        },
			{ "&&", TokenType::TOKEN_DOUBLE_AMPERSAND },
			{ "|",  TokenType::TOKEN_PIPE             },
			{ "||", TokenType::TOKEN_DOUBLE_PIPE      },
		};

		c = advance();

		// At maximum special tokens are a combination of two characters.
		if (!is_eof(c) && !is_whitespace(c)) _current_offset++;

		const auto current_token = this->current_token();
		if (k_tokens.contains(current_token)) return Token(k_tokens.at(current_token));

		diagnostic(DiagnosticCode::ERROR_LEXER_UNRECOGNIZED_TOKEN);
		return std::nullopt;
	}

	Token Lexer::create_identifier_token()
	{
		while (is_alpha(peek())) advance();

		// Keywords
		static const std::unordered_map<std::string_view, TokenType> k_keywords = {
			// Keywords
			{ "break",    TokenType::TOKEN_BREAK    },
            { "continue", TokenType::TOKEN_CONTINUE },
			{ "else",     TokenType::TOKEN_ELSE     },
            { "false",    TokenType::TOKEN_FALSE    },
			{ "fn",       TokenType::TOKEN_FN       },
            { "for",      TokenType::TOKEN_FOR      },
			{ "if",       TokenType::TOKEN_IF       },
            { "let",      TokenType::TOKEN_LET      },
			{ "mut",      TokenType::TOKEN_MUT      },
            { "native",   TokenType::TOKEN_NATIVE   },
			{ "return",   TokenType::TOKEN_RETURN   },
            { "struct",   TokenType::TOKEN_STRUCT   },
			{ "true",     TokenType::TOKEN_TRUE     },
            { "while",    TokenType::TOKEN_WHILE    },
		};

		const auto current_token = this->current_token();
		if (k_keywords.contains(current_token)) return Token(k_keywords.at(current_token));

		// Identifier
		return Token(TokenType::TOKEN_LITERAL_IDENTIFIER, std::string(current_token));
	}

	// TODO(sand_noodles):
	//   This method handles only Integers and Floats.
	//   Add support for scanning:
	//   - binary (ex. 0x0111b)
	//   - hex (ex. 0xFF)
	std::optional<Token> Lexer::create_numeric_token()
	{
		char c = peek();
		if (peek() == '-') c = advance();

		while (!is_eof(c) && (is_hex_digit(c) || c == '.' || c == 'x')) {
			c = advance();
		}

		const auto create_token = [this]<is_value_t T>(std::string_view token, TokenType type) -> std::optional<Token> {
			T value                    = 0;
			const auto [_, error_code] = std::from_chars(token.data(), token.data() + token.size(), value);

			if (error_code == std::errc{}) [[likely]] {
				return Token(type, value);  // No error.
			} else if (error_code == std::errc::invalid_argument) {
				diagnostic(DiagnosticCode::ERROR_LEXER_VALUE_IS_NOT_A_NUMBER);
				return std::nullopt;
			} else if (error_code == std::errc::result_out_of_range) {
				diagnostic(DiagnosticCode::ERROR_LEXER_VALUE_OUT_OF_RANGE);
				return std::nullopt;
			}

			diagnostic(DiagnosticCode::ERROR_LEXER_UNRECOGNIZED_TOKEN);
			return std::nullopt;
		};

		const auto current_token = this->current_token();
		return current_token.find('.') != std::string_view::npos
		         ? create_token.operator()<double>(current_token, TokenType::TOKEN_LITERAL_FLOAT)
		         : create_token.operator()<int64_t>(current_token, TokenType::TOKEN_LITERAL_INTEGER);
	}

	std::optional<Token> Lexer::create_string_token()
	{
		char_t c = advance();  // Skip "
		while (!is_quote(c) && !is_eof(c)) {
			c = advance();
		}

		if (is_eof(c)) {
			diagnostic(DiagnosticCode::ERROR_LEXER_UNTERMINATED_STRING);
			return std::nullopt;
		}

		const auto current_token = _script.substr(_start_index + 1, _current_offset - _start_index - 1);
		advance();
		return Token(TokenType::TOKEN_LITERAL_STRING, std::string(current_token));
	}

	void Lexer::skip_whitespace()
	{
		for (;;) {
			char_t c = peek();
			switch (c) {
				case ' ':
				case '\r':
				case '\t':
				case '\n':
					advance();
					break;
				case '#':
					advance();  // Skip the '#'
					c = peek();
					while (!is_eof(c) && (c != '\n' || c != '\r')) {
						advance();  // Skip the '#'
						c = peek();
					}
					break;
				default:
					return;
			}
		}
	}

	Lexer::char_t Lexer::peek(size_t count) const
	{
		if (_current_offset + count >= _script.size()) {
			return '\0';
		}
		return _script[_current_offset + count];
	}

	Lexer::char_t Lexer::advance()
	{
		_current_offset++;
		return peek();
	}

	std::string_view Lexer::current_token() const
	{
		return _script.substr(_start_index, _current_offset - _start_index);
	}

	//
	//
	//

	static bool is_eof(Lexer::char_t c) { return c == '\0'; }

	static bool is_whitespace(Lexer::char_t c) { return c == ' ' || c == '\t' || c == '\n' || c == '\r'; }

	static bool is_alpha(Lexer::char_t c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_'; }

	static bool is_digit(Lexer::char_t c) { return c >= '0' && c <= '9'; }

	static bool is_hex_digit(Lexer::char_t c)
	{
		return is_digit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
	}

	static bool is_quote(Lexer::char_t c) { return c == '"' || c == '\''; }
}  // namespace soul
