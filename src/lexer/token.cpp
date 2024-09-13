#include "lexer/token.h"

#include <unordered_map>
#include <format>
#include <utility>

namespace soul
{
	std::string_view token_type_to_string_view(TokenType type)
	{
		static const std::unordered_map<TokenType, std::string_view> k_types = {
			{TokenType::TOKEN_UNKNOWN,            "token_unknown" },
			{TokenType::TOKEN_SEMICOLON,          "token_semicolon" },
			{TokenType::TOKEN_QUESTION_MARK,      "token_question_mark" },
			{TokenType::TOKEN_PERCENT,            "token_percent" },
			{TokenType::TOKEN_CARET,              "token_caret" },
			{TokenType::TOKEN_DOT,                "token_dot" },
			{TokenType::TOKEN_COMMA,              "token_comma" },
			{TokenType::TOKEN_PAREN_LEFT,         "token_paren_left" },
			{TokenType::TOKEN_PAREN_RIGHT,        "token_paren_right" },
			{TokenType::TOKEN_BRACE_LEFT,         "token_brace_left" },
			{TokenType::TOKEN_BRACE_RIGHT,        "token_brace_right" },
			{TokenType::TOKEN_BRACKET_LEFT,       "token_bracket_left" },
			{TokenType::TOKEN_BRACKET_RIGHT,      "token_bracket_right" },
			{TokenType::TOKEN_COLON,              "token_colon" },
			{TokenType::TOKEN_DOUBLE_COLON,       "token_double_colon" },
			{TokenType::TOKEN_EQUAL,              "token_equal" },
			{TokenType::TOKEN_DOUBLE_EQUAL,       "token_double_equal" },
			{TokenType::TOKEN_BANG,               "token_bang" },
			{TokenType::TOKEN_BANG_EQUAL,         "token_bang_equal" },
			{TokenType::TOKEN_GREATER,            "token_greater" },
			{TokenType::TOKEN_GREATER_EQUAL,      "token_greater_equal"},
			{TokenType::TOKEN_LESS,               "token_less" },
			{TokenType::TOKEN_LESS_EQUAL,         "token_less_equal" },
			{TokenType::TOKEN_PLUS,               "token_plus" },
			{TokenType::TOKEN_PLUS_EQUAL,         "token_plus_equal" },
			{TokenType::TOKEN_DOUBLE_PLUS,        "token_double_plus" },
			{TokenType::TOKEN_MINUS,              "token_minus" },
			{TokenType::TOKEN_MINUS_EQUAL,        "token_minus_equal" },
			{TokenType::TOKEN_DOUBLE_MINUS,       "token_double_minus" },
			{TokenType::TOKEN_STAR,               "token_star" },
			{TokenType::TOKEN_STAR_EQUAL,         "token_star_equal" },
			{TokenType::TOKEN_SLASH,              "token_slash" },
			{TokenType::TOKEN_SLASH_EQUAL,        "token_slash_equal" },
			{TokenType::TOKEN_AMPERSAND,          "token_ampersand" },
			{TokenType::TOKEN_DOUBLE_AMPERSAND,   "token_double_ampersand" },
			{TokenType::TOKEN_PIPE,               "token_pipe" },
			{TokenType::TOKEN_DOUBLE_PIPE,        "token_double_pipe" },
			{TokenType::TOKEN_LITERAL_INTEGER,    "token_literal_integer" },
			{TokenType::TOKEN_LITERAL_FLOAT,      "token_literal_float" },
			{TokenType::TOKEN_LITERAL_STRING,     "token_literal_string" },
			{TokenType::TOKEN_LITERAL_IDENTIFIER, "token_literal_identifier" },
			{TokenType::TOKEN_NATIVE,             "token_native" },
			{TokenType::TOKEN_LET,                "token_let" },
			{TokenType::TOKEN_MUT,                "token_mut" },
			{TokenType::TOKEN_IF,                 "token_if" },
			{TokenType::TOKEN_ELSE,               "token_else" },
			{TokenType::TOKEN_FOR,                "token_for" },
			{TokenType::TOKEN_WHILE,              "token_while" },
			{TokenType::TOKEN_CONTINUE,           "token_continue" },
			{TokenType::TOKEN_BREAK,              "token_break" },
			{TokenType::TOKEN_RETURN,             "token_return" },
			{TokenType::TOKEN_FN,                 "token_function" },
			{TokenType::TOKEN_STRUCT,             "token_struct" },
			{TokenType::TOKEN_TRUE,               "token_true" },
			{TokenType::TOKEN_FALSE,              "token_false" },
			{TokenType::TOKEN_EOF,                "[EOF]" },
		};
		if (!k_types.contains(type)) [[unlikely]] {
			return k_types.at(TokenType::TOKEN_UNKNOWN);
		}
		return k_types.at(type);
	}

	Token::Token(TokenType type, value_t value)
		: _type(type), _value(std::move(value)) {}

	Token::operator std::string() const
	{
		const auto& value_string = std::visit([](const auto& value) -> std::string {
				if constexpr (std::is_same_v<std::remove_cvref_t<decltype(value)>, empty_t>)
				{
					return std::string("[no_value]");
				} else if constexpr (std::is_same_v<std::remove_cvref_t<decltype(value)>, std::string>){
					return value;
				} else {
					return std::to_string(value);
				}
			}, _value);
		return std::string(std::format("[type: {}, value: {}]",
		                               token_type_to_string_view(_type), value_string));
	}
}
