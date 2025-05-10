#include "lexer/lexer.h"

#include <algorithm>
#include <charconv>
#include <iostream>
#include <unordered_map>

#define MATCH_SINGLE_CHARACTER(character, token_type) \
	case character:                                   \
	{                                                 \
		return Token(token_type);                     \
	}

#define MATCH_TWO_CHARACTERS(first_character, second_character, single_match, both_match) \
	case first_character:                                                                 \
	{                                                                                     \
		if (peek() == second_character) {                                                 \
			std::ignore = advance();                                                      \
			return Token(TokenType(both_match));                                          \
		}                                                                                 \
		return Token(single_match);                                                       \
	}

#define MATCH_TWO_CHARACTERS_SPECIAL(                                                            \
	first_character, second_char_a, second_char_b, single_match, double_match_a, double_match_b) \
	case first_character:                                                                        \
	{                                                                                            \
		switch (peek()) {                                                                        \
			case second_char_a:                                                                  \
				std::ignore = advance();                                                         \
				return Token(double_match_a);                                                    \
			case second_char_b:                                                                  \
				std::ignore = advance();                                                         \
				return Token(double_match_b);                                                    \
			default:                                                                             \
				break;                                                                           \
		}                                                                                        \
		return Token(single_match);                                                              \
	}

namespace soul::lexer
{
	Lexer::Lexer(std::string_view script, Diagnostics* diagnostics)
		: _script(script), _start_index(0), _current_offset(0), _diagnostics(diagnostics)
	{
	}

	std::vector<Token> Lexer::tokenize(std::string_view script, Diagnostics* diagnostics)
	{
		return Lexer{ script, diagnostics }.tokenize();
	}

	std::vector<Token> Lexer::tokenize()
	{
		if (_script.empty()) {
			return { Token(TokenType::EndOfFile) };
		}

		std::vector<Token> result;
		for (;;) {
			skip_whitespace();
			auto result_token = scan_token();
			if (!result_token) {
				break;
			}
			result.push_back(std::move(*result_token));
		}
		result.emplace_back(TokenType::EndOfFile);
		return result;
	}

	std::optional<Token> Lexer::scan_token()
	{
		_start_index = _current_offset;

		auto current_char = peek(0);

		if (is_eof(current_char)) {
			return std::nullopt;
		}

		if (is_alpha(current_char)) {
			return create_identifier_token();
		}

		if (is_digit(current_char) || current_char == '-' && is_digit(peek(1))) {
			return create_numeric_token();
		}

		if (is_quote(current_char)) {
			return create_string_token();
		}

		// Special characters
		std::ignore = advance();
		switch (current_char) {
			// Simple cases, where there are no other characters to consider afterward.
			MATCH_SINGLE_CHARACTER(';', TokenType::Semicolon)
			MATCH_SINGLE_CHARACTER('?', TokenType::QuestionMark)
			MATCH_SINGLE_CHARACTER('%', TokenType::Percent)
			MATCH_SINGLE_CHARACTER('^', TokenType::Caret)
			MATCH_SINGLE_CHARACTER('.', TokenType::Dot)
			MATCH_SINGLE_CHARACTER(',', TokenType::Comma)
			MATCH_SINGLE_CHARACTER('(', TokenType::ParenLeft)
			MATCH_SINGLE_CHARACTER(')', TokenType::ParenRight)
			MATCH_SINGLE_CHARACTER('{', TokenType::BraceLeft)
			MATCH_SINGLE_CHARACTER('}', TokenType::BraceRight)
			MATCH_SINGLE_CHARACTER('[', TokenType::BracketLeft)
			MATCH_SINGLE_CHARACTER(']', TokenType::BracketRight)

			// Two character tokens.
			MATCH_TWO_CHARACTERS(':', ':', TokenType::Colon, TokenType::DoubleColon)
			MATCH_TWO_CHARACTERS('=', '=', TokenType::Equal, TokenType::DoubleEqual)
			MATCH_TWO_CHARACTERS('!', '=', TokenType::Bang, TokenType::BangEqual)
			MATCH_TWO_CHARACTERS('>', '=', TokenType::Greater, TokenType::GreaterEqual)
			MATCH_TWO_CHARACTERS('<', '=', TokenType::Less, TokenType::LessEqual)
			MATCH_TWO_CHARACTERS('*', '=', TokenType::Star, TokenType::StarEqual)
			MATCH_TWO_CHARACTERS('/', '=', TokenType::Slash, TokenType::SlashEqual)
			MATCH_TWO_CHARACTERS('&', '&', TokenType::Ampersand, TokenType::DoubleAmpersand)
			MATCH_TWO_CHARACTERS('|', '|', TokenType::Pipe, TokenType::DoublePipe)

			// Special cases - second character might vary.
			MATCH_TWO_CHARACTERS_SPECIAL('+', '+', '=', TokenType::Plus, TokenType::DoublePlus, TokenType::PlusEqual)
			MATCH_TWO_CHARACTERS_SPECIAL('-', '-', '=', TokenType::Minus, TokenType::DoubleMinus, TokenType::MinusEqual)
			default:
				break;
		}

		diagnostic(DiagnosticType::Error, DiagnosticCode::LexerUnrecognizedToken);
		return std::nullopt;
	}

	Token Lexer::create_identifier_token()
	{
		while (is_alpha(peek()) || is_digit(peek())) advance();

		// Keywords
		static const std::unordered_map<std::string_view, TokenType> k_keywords = {
			// Keywords
			{ "break",    TokenType::KeywordBreak      },
			{ "cast",     TokenType::KeywordCast       },
			{ "continue", TokenType::KeywordContinue   },
			{ "else",     TokenType::KeywordElse       },
			{ "false",    TokenType::KeywordFalse      },
			{ "fn",       TokenType::KeywordFn         },
			{ "for",      TokenType::KeywordFor        },
			{ "foreach",  TokenType::KeywordForeach    },
			{ "in",       TokenType::KeywordIn         },
			{ "if",       TokenType::KeywordIf         },
			{ "let",      TokenType::KeywordLet        },
			{ "mut",      TokenType::KeywordMut        },
			{ "native",   TokenType::KeywordNative     },
			{ "return",   TokenType::KeywordReturn     },
			{ "struct",   TokenType::KeywordStruct     },
			{ "true",     TokenType::KeywordTrue       },
			{ "while",    TokenType::KeywordWhile      },

			// (Explicit) Primitive types
			{ "i32",      TokenType::LiteralIdentifier },
			{ "i64",      TokenType::LiteralIdentifier },
			{ "f32",      TokenType::LiteralIdentifier },
			{ "f64",      TokenType::LiteralIdentifier },
			{ "str",      TokenType::LiteralIdentifier },
			{ "chr",      TokenType::LiteralIdentifier },
			{ "bool",     TokenType::LiteralIdentifier },
		};

		auto current_token = this->current_token();
		if (k_keywords.contains(current_token)) {
			const auto type = k_keywords.at(current_token);
			return Token(type, type == TokenType::LiteralIdentifier ? Value{ std::string(current_token) } : Value{});
		}

		// Identifier
		return Token(TokenType::LiteralIdentifier, Value(std::string(current_token)));
	}

	// TODO(sand_noodles):
	//   This method handles only Integers and Floats.
	//   Add support for scanning:
	//   - binary (ex. 0x0111b)
	//   - hex (ex. 0xFF)
	std::optional<Token> Lexer::create_numeric_token()
	{
		// Create (potentially) numeric token.
		auto c = peek();
		while (!is_eof(c) && is_hex_digit(c) || c == '-' || c == '.' || c == 'x') {
			c = advance();
		}

		const auto create_token = [this]<ValueKind T>(std::string_view token, TokenType type) -> std::optional<Token> {
			T value                    = 0;
			const auto [_, error_code] = std::from_chars(token.data(), token.data() + token.size(), value);

			if (error_code == std::errc{}) [[likely]] {
				return Token(type, Value(value));  // No error.
			} else if (error_code == std::errc::invalid_argument) {
				diagnostic(DiagnosticType::Error, DiagnosticCode::LexerValueIsNotANumber);
				return std::nullopt;
			} else if (error_code == std::errc::result_out_of_range) {
				diagnostic(DiagnosticType::Error, DiagnosticCode::LexerValueOutOfRange);
				return std::nullopt;
			}

			diagnostic(DiagnosticType::Error, DiagnosticCode::LexerUnrecognizedToken);
			return std::nullopt;
		};

		const auto current_token = this->current_token();
		return current_token.find('.') != std::string_view::npos
		         ? create_token.operator()<f64>(current_token, TokenType::LiteralFloat)
		         : create_token.operator()<i64>(current_token, TokenType::LiteralInteger);
	}

	std::optional<Token> Lexer::create_string_token()
	{
		std::ignore       = advance();  // Skip over the quotation.
		auto current_char = peek();
		while (!(is_quote(current_char) || is_eof(current_char))) {
			current_char = advance();
		}

		if (is_eof(current_char)) {
			diagnostic(DiagnosticType::Error, DiagnosticCode::LexerUnterminatedString);
			return std::nullopt;
		}

		std::ignore              = advance();  // Skip over the quotation.
		const auto current_token = _script.substr(_start_index + 1, _current_offset - _start_index - 2);  // Exclude '"'
		return Token(TokenType::LiteralString, Value(std::string(current_token)));
	}

	std::string_view Lexer::current_token() const
	{
		return _script.substr(_start_index, _current_offset - _start_index);
	}

	void Lexer::skip_whitespace()
	{
		for (;;) {
			auto current_char = peek();
			switch (current_char) {
				case ' ':
				case '\r':
				case '\t':
				case '\n':
					std::ignore = advance();
					break;
				case '#':
					std::ignore  = advance();  // Skip the '#'
					current_char = peek();
					while (!is_eof(current_char) && (current_char != '\n' || current_char != '\r')) {
						std::ignore  = advance();  // Skip the '#'
						current_char = peek();
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
}  // namespace soul::lexer
