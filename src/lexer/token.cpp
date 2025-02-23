#include "lexer/token.h"

#include <format>
#include <unordered_map>
#include <utility>

namespace soul
{
	std::string_view to_string(TokenType type)
	{
		static const std::unordered_map<TokenType, std::string_view> k_types = {
			{ TokenType::Unknown,           "token_unknown"            },
			{ TokenType::Semicolon,         "token_semicolon"          },
			{ TokenType::QuestionMark,      "token_question_mark"      },
			{ TokenType::Percent,           "token_percent"            },
			{ TokenType::Caret,             "token_caret"              },
			{ TokenType::Dot,               "token_dot"                },
			{ TokenType::Comma,             "token_comma"              },
			{ TokenType::ParenLeft,         "token_paren_left"         },
			{ TokenType::ParenRight,        "token_paren_right"        },
			{ TokenType::BraceLeft,         "token_brace_left"         },
			{ TokenType::BraceRight,        "token_brace_right"        },
			{ TokenType::BracketLeft,       "token_bracket_left"       },
			{ TokenType::BracketRight,      "token_bracket_right"      },
			{ TokenType::Colon,             "token_colon"              },
			{ TokenType::DoubleColon,       "token_double_colon"       },
			{ TokenType::Equal,             "token_equal"              },
			{ TokenType::DoubleEqual,       "token_double_equal"       },
			{ TokenType::Bang,              "token_bang"               },
			{ TokenType::BangEqual,         "token_bang_equal"         },
			{ TokenType::Greater,           "token_greater"            },
			{ TokenType::GreaterEqual,      "token_greater_equal"      },
			{ TokenType::Less,              "token_less"               },
			{ TokenType::LessEqual,         "token_less_equal"         },
			{ TokenType::Plus,              "token_plus"               },
			{ TokenType::PlusEqual,         "token_plus_equal"         },
			{ TokenType::DoublePlus,        "token_double_plus"        },
			{ TokenType::Minus,             "token_minus"              },
			{ TokenType::MinusEqual,        "token_minus_equal"        },
			{ TokenType::DoubleMinus,       "token_double_minus"       },
			{ TokenType::Star,              "token_star"               },
			{ TokenType::StarEqual,         "token_star_equal"         },
			{ TokenType::Slash,             "token_slash"              },
			{ TokenType::SlashEqual,        "token_slash_equal"        },
			{ TokenType::Ampersand,         "token_ampersand"          },
			{ TokenType::DoubleAmpersand,   "token_double_ampersand"   },
			{ TokenType::Pipe,              "token_pipe"               },
			{ TokenType::DoublePipe,        "token_double_pipe"        },
			{ TokenType::LiteralInteger,    "token_literal_integer"    },
			{ TokenType::LiteralFloat,      "token_literal_float"      },
			{ TokenType::LiteralString,     "token_literal_string"     },
			{ TokenType::LiteralIdentifier, "token_literal_identifier" },
			{ TokenType::KeywordNative,     "token_native"             },
			{ TokenType::KeywordLet,        "token_let"                },
			{ TokenType::KeywordMut,        "token_mut"                },
			{ TokenType::KeywordIn,         "token_in"                 },
			{ TokenType::KeywordIf,         "token_if"                 },
			{ TokenType::KeywordElse,       "token_else"               },
			{ TokenType::KeywordFor,        "token_for"                },
			{ TokenType::KeywordForeach,    "token_foreach"            },
			{ TokenType::KeywordWhile,      "token_while"              },
			{ TokenType::KeywordContinue,   "token_continue"           },
			{ TokenType::KeywordBreak,      "token_break"              },
			{ TokenType::KeywordReturn,     "token_return"             },
			{ TokenType::KeywordFn,         "token_function"           },
			{ TokenType::KeywordStruct,     "token_struct"             },
			{ TokenType::KeywordTrue,       "token_true"               },
			{ TokenType::KeywordFalse,      "token_false"              },
			{ TokenType::EndOfFile,         "[EOF]"                    },
		};
		if (!k_types.contains(type)) [[unlikely]] {
			return k_types.at(TokenType::Unknown);
		}
		return k_types.at(type);
	}

	Token::Token(TokenType type, ValueType value) : _type(type), _value(std::move(value)) {}

	Token::operator std::string() const
	{
		const auto& value_string = std::visit(
			[](const auto& value) -> std::string {
				if constexpr (std::is_same_v<std::remove_cvref_t<decltype(value)>, std::monostate>) {
					return std::string("[no_value]");
				} else if constexpr (std::is_same_v<std::remove_cvref_t<decltype(value)>, std::string>) {
					return value;
				} else {
					return std::to_string(value);
				}
			},
			_value);
		return std::string(std::format("[type: {}, value: {}]", to_string(_type), value_string));
	}

	[[nodiscard]] TokenType Token::type() const { return _type; }

	[[nodiscard]] bool Token::is_type(const TokenType type) const noexcept { return _type == type; }

	[[nodiscard]] bool Token::is_one_of_type(const TokenType t1, const TokenType t2) const noexcept
	{
		return _type == t1 || _type == t2;
	}
}  // namespace soul
