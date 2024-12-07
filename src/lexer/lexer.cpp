#include "lexer/lexer.h"

#include <algorithm>
#include <charconv>
#include <unordered_map>

namespace soul
{
	[[nodiscard]] std::vector<Token> Lexer::tokenize(std::string_view script)
	{
		_current_offset = 0;
		_start_index    = 0;
		_script         = script;
		_diagnostics.clear();

		if (script.empty()) {
			return { Token(TokenType::EndOfFile) };
		}

		std::vector<Token> result;
		for (;;) {
			auto result_token = scan_token();
			if (!result_token.has_value() || result_token->type() == TokenType::EndOfFile) break;
			result.push_back(std::move(*result_token));
		}
		result.emplace_back(TokenType::EndOfFile);
		return result;
	}

	std::optional<Token> Lexer::scan_token()
	{
		skip_whitespace();

		_start_index = _current_offset;
		Char c       = peek();

		if (is_eof(c)) return Token(TokenType::EndOfFile);

		if (is_alpha(c)) return create_identifier_token();

		if (is_digit(c) || (c == '-' && is_digit(peek(1)))) return create_numeric_token();

		if (is_quote(c)) return create_string_token();

		// Special characters
		static const std::unordered_map<std::string_view, TokenType> k_tokens = {
			// Single character tokens
			{ ";",  TokenType::Semicolon       },
			{ "?",  TokenType::QuestionMark    },
			{ "%",  TokenType::Percent         },
			{ "^",  TokenType::Caret           },
			{ ".",  TokenType::Dot             },
			{ ",",  TokenType::Comma           },
			{ "(",  TokenType::ParenLeft       },
			{ ")",  TokenType::ParenRight      },
			{ "{",  TokenType::BraceLeft       },
			{ "}",  TokenType::BraceRight      },
			{ "[",  TokenType::BracketLeft     },
			{ "]",  TokenType::BracketRight    },

			// One or two character tokens
			{ ":",  TokenType::Colon           },
			{ "::", TokenType::DoubleColon     },
			{ "=",  TokenType::Equal           },
			{ "==", TokenType::DoubleEqual     },
			{ "!",  TokenType::Bang            },
			{ "!=", TokenType::BangEqual       },
			{ ">",  TokenType::Greater         },
			{ ">=", TokenType::GreaterEqual    },
			{ "<",  TokenType::Less            },
			{ "<=", TokenType::LessEqual       },
			{ "+",  TokenType::Plus            },
			{ "+=", TokenType::PlusEqual       },
			{ "++", TokenType::DoublePlus      },
			{ "-",  TokenType::Minus           },
			{ "-=", TokenType::MinusEqual      },
			{ "--", TokenType::DoubleMinus     },
			{ "*",  TokenType::Star            },
			{ "*=", TokenType::StarEqual       },
			{ "/",  TokenType::Slash           },
			{ "/=", TokenType::SlashEqual      },
			{ "&",  TokenType::Ampersand       },
			{ "&&", TokenType::DoubleAmpersand },
			{ "|",  TokenType::Pipe            },
			{ "||", TokenType::DoublePipe      },
		};

		c = advance();

		// At maximum special tokens are a combination of two characters.
		if (!is_eof(c) && !is_whitespace(c)) _current_offset++;

		const auto current_token = this->current_token();
		if (k_tokens.contains(current_token)) return Token(k_tokens.at(current_token));

		diagnostic(DiagnosticCode::ErrorLexerUnrecognizedToken);
		return std::nullopt;
	}

	Token Lexer::create_identifier_token()
	{
		while (is_alpha(peek())) advance();

		// Keywords
		static const std::unordered_map<std::string_view, TokenType> k_keywords = {
			// Keywords
			{ "break",    TokenType::KeywordBreak    },
            { "continue", TokenType::KeywordContinue },
			{ "else",     TokenType::KeywordElse     },
            { "false",    TokenType::KeywordFalse    },
			{ "fn",       TokenType::KeywordFn       },
            { "for",      TokenType::KeywordFor      },
			{ "foreach",  TokenType::KeywordForeach  },
            { "in",       TokenType::KeywordIn       },
			{ "if",       TokenType::KeywordIf       },
            { "let",      TokenType::KeywordLet      },
			{ "mut",      TokenType::KeywordMut      },
            { "native",   TokenType::KeywordNative   },
			{ "return",   TokenType::KeywordReturn   },
            { "struct",   TokenType::KeywordStruct   },
			{ "true",     TokenType::KeywordTrue     },
            { "while",    TokenType::KeywordWhile    },
		};

		const auto current_token = this->current_token();
		if (k_keywords.contains(current_token)) return Token(k_keywords.at(current_token));

		// Identifier
		return Token(TokenType::LiteralIdentifier, std::string(current_token));
	}

	// TODO(sand_noodles):
	//   This method handles only Integers and Floats.
	//   Add support for scanning:
	//   - binary (ex. 0x0111b)
	//   - hex (ex. 0xFF)
	std::optional<Token> Lexer::create_numeric_token()
	{
		// Create (potentially) numeric token.
		char c = peek();
		if (peek() == '-') c = advance();
		while (!is_eof(c) && (is_hex_digit(c) || c == '.' || c == 'x')) {
			c = advance();
		}

		const auto create_token = [this]<ValueKind T>(std::string_view token, TokenType type) -> std::optional<Token> {
			T value                    = 0;
			const auto [_, error_code] = std::from_chars(token.data(), token.data() + token.size(), value);

			if (error_code == std::errc{}) [[likely]] {
				return Token(type, value);  // No error.
			} else if (error_code == std::errc::invalid_argument) {
				diagnostic(DiagnosticCode::ErrorLexerValueIsNotANumber);
				return std::nullopt;
			} else if (error_code == std::errc::result_out_of_range) {
				diagnostic(DiagnosticCode::ErrorLexerValueOutOfRange);
				return std::nullopt;
			}

			diagnostic(DiagnosticCode::ErrorLexerUnrecognizedToken);
			return std::nullopt;
		};

		const auto current_token = this->current_token();
		return current_token.find('.') != std::string_view::npos
		         ? create_token.operator()<f64>(current_token, TokenType::LiteralFloat)
		         : create_token.operator()<i64>(current_token, TokenType::LiteralInteger);
	}

	std::optional<Token> Lexer::create_string_token()
	{
		Char c = advance();  // Skip "
		while (!is_quote(c) && !is_eof(c)) {
			c = advance();
		}

		if (is_eof(c)) {
			diagnostic(DiagnosticCode::ErrorLexerUnterminatedString);
			return std::nullopt;
		}

		const auto current_token = _script.substr(_start_index + 1, _current_offset - _start_index - 1);
		advance();
		return Token(TokenType::LiteralString, std::string(current_token));
	}

	std::string_view Lexer::current_token() const
	{
		return _script.substr(_start_index, _current_offset - _start_index);
	}

	void Lexer::skip_whitespace()
	{
		for (;;) {
			Char c = peek();
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

	Lexer::Char Lexer::peek(size_t count) const
	{
		if (_current_offset + count >= _script.size()) {
			return '\0';
		}
		return _script[_current_offset + count];
	}

	Lexer::Char Lexer::advance()
	{
		_current_offset++;
		return peek();
	}

	bool Lexer::is_eof(Lexer::Char c) { return c == '\0'; }
	bool Lexer::is_whitespace(Lexer::Char c) { return c == ' ' || c == '\t' || c == '\n' || c == '\r'; }
	bool Lexer::is_alpha(Lexer::Char c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_'; }
	bool Lexer::is_digit(Lexer::Char c) { return c >= '0' && c <= '9'; }
	bool Lexer::is_hex_digit(Lexer::Char c) { return is_digit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'); }
	bool Lexer::is_quote(Lexer::Char c) { return c == '"' || c == '\''; }
}  // namespace soul
