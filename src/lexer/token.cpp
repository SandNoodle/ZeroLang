#include "lexer/token.h"

#include <unordered_map>
#include <format>

namespace soul
{
	std::string_view token_type_to_string(token_type_t type)
	{
		static const std::unordered_map<token_type_t, std::string_view> types = {
			{ token_type_t::token_unknown,            "token_unknown" },
			{ token_type_t::token_semicolon,          "token_semicolon" },
			{ token_type_t::token_question_mark,      "token_question_mark" },
			{ token_type_t::token_percent,            "token_percent" },
			{ token_type_t::token_caret,              "token_caret" },
			{ token_type_t::token_dot,                "token_dot" },
			{ token_type_t::token_comma,              "token_comma" },
			{ token_type_t::token_paren_left,         "token_paren_left" },
			{ token_type_t::token_paren_right,        "token_paren_right" },
			{ token_type_t::token_brace_left,         "token_brace_left" },
			{ token_type_t::token_brace_right,        "token_brace_right" },
			{ token_type_t::token_bracket_left,       "token_bracket_left" },
			{ token_type_t::token_bracket_right,      "token_bracket_right" },
			{ token_type_t::token_colon,              "token_colon" },
			{ token_type_t::token_double_colon,       "token_double_colon" },
			{ token_type_t::token_equal,              "token_equal" },
			{ token_type_t::token_double_equal,       "token_double_equal" },
			{ token_type_t::token_bang,               "token_bang" },
			{ token_type_t::token_bang_equal,         "token_bang_equal" },
			{ token_type_t::token_greater,            "token_greater" },
			{ token_type_t::token_greater_equal,      "token_greater_equal"},
			{ token_type_t::token_less,               "token_less" },
			{ token_type_t::token_less_equal,         "token_less_equal" },
			{ token_type_t::token_plus,               "token_plus" },
			{ token_type_t::token_plus_equal,         "token_plus_equal" },
			{ token_type_t::token_double_plus,        "token_double_plus" },
			{ token_type_t::token_minus,              "token_minus" },
			{ token_type_t::token_minus_equal,        "token_minus_equal" },
			{ token_type_t::token_double_minus,       "token_double_minus" },
			{ token_type_t::token_star,               "token_star" },
			{ token_type_t::token_star_equal,         "token_star_equal" },
			{ token_type_t::token_slash,              "token_slash" },
			{ token_type_t::token_slash_equal,        "token_slash_equal" },
			{ token_type_t::token_ampersand,          "token_ampersand" },
			{ token_type_t::token_double_ampersand,   "token_double_ampersand" },
			{ token_type_t::token_pipe,               "token_pipe" },
			{ token_type_t::token_double_pipe,        "token_double_pipe" },
			{ token_type_t::token_literal_integer,    "token_literal_integer" },
			{ token_type_t::token_literal_float,      "token_literal_float" },
			{ token_type_t::token_literal_string,     "token_literal_string" },
			{ token_type_t::token_literal_identifier, "token_literal_identifier" },
			{ token_type_t::token_native,             "token_native" },
			{ token_type_t::token_let,                "token_let" },
			{ token_type_t::token_mut,                "token_mut" },
			{ token_type_t::token_if,                 "token_if" },
			{ token_type_t::token_else,               "token_else" },
			{ token_type_t::token_for,                "token_for" },
			{ token_type_t::token_while,              "token_while" },
			{ token_type_t::token_continue,           "token_continue" },
			{ token_type_t::token_break,              "token_break" },
			{ token_type_t::token_return,             "token_return" },
			{ token_type_t::token_fn,                 "token_function" },
			{ token_type_t::token_struct,             "token_struct" },
			{ token_type_t::token_true,               "token_true" },
			{ token_type_t::token_false,              "token_false" },
			{ token_type_t::token_error,              "[ERROR]" },
			{ token_type_t::token_eof,                "[EOF]" },
		};
		if (!types.contains(type)) [[unlikely]] {
			return types.at(token_type_t::token_unknown);
		}
		return types.at(type);
	}

	token_t::token_t(token_type_t type, const value_t& value)
		: _type(type), _value(value) {}

	token_t::operator std::string() const
	{
		const auto& type_string = token_type_to_string(_type);
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
		return std::string(std::format("[type: {}, value: {}]", token_type_to_string(_type), value_string));
	}
}
