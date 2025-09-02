#include "lexer/token.h"

#include <unordered_map>

namespace soul
{
	std::string_view Token::name(Token::Type type) noexcept
	{
		using namespace std::string_view_literals;
		static const std::unordered_map<Token::Type, std::string_view> k_token_name = {
			{ Token::Type::KeywordBreak,             "keyword_break"sv              },
			{ Token::Type::KeywordCast,              "keyword_cast"sv               },
			{ Token::Type::KeywordContinue,          "keyword_continue"sv           },
			{ Token::Type::KeywordElse,              "keyword_else"sv               },
			{ Token::Type::KeywordFalse,             "keyword_false"sv              },
			{ Token::Type::KeywordFn,                "keyword_fn"sv                 },
			{ Token::Type::KeywordFor,               "keyword_for"sv                },
			{ Token::Type::KeywordIf,                "keyword_if"sv                 },
			{ Token::Type::KeywordLet,               "keyword_let"sv                },
			{ Token::Type::KeywordMut,               "keyword_mut"sv                },
			{ Token::Type::KeywordNative,            "keyword_native"sv             },
			{ Token::Type::KeywordReturn,            "keyword_return"sv             },
			{ Token::Type::KeywordStruct,            "keyword_struct"sv             },
			{ Token::Type::KeywordTrue,              "keyword_true"sv               },
			{ Token::Type::KeywordWhile,             "keyword_while"sv              },
			{ Token::Type::LiteralFloat,             "literal_float"sv              },
			{ Token::Type::LiteralIdentifier,        "literal_identifier"sv         },
			{ Token::Type::LiteralInteger,           "literal_integer"sv            },
			{ Token::Type::LiteralString,            "literal_string"sv             },
			{ Token::Type::SymbolAmpersand,          "symbol_ampersand"sv           },
			{ Token::Type::SymbolAmpersandAmpersand, "symbol_ampersand_ampersand"sv },
			{ Token::Type::SymbolCaret,              "symbol_caret"sv               },
			{ Token::Type::SymbolComma,              "symbol_comma"sv               },
			{ Token::Type::SymbolDot,                "symbol_dot"sv                 },
			{ Token::Type::SymbolGreater,            "symbol_greater"sv             },
			{ Token::Type::SymbolGreaterEqual,       "symbol_greater_equal"sv       },
			{ Token::Type::SymbolLess,               "symbol_less"sv                },
			{ Token::Type::SymbolLessEqual,          "symbol_less_equal"sv          },
			{ Token::Type::SymbolMinus,              "symbol_minus"sv               },
			{ Token::Type::SymbolMinusEqual,         "symbol_minus_equal"sv         },
			{ Token::Type::SymbolMinusMinus,         "symbol_minus_minus"sv         },
			{ Token::Type::SymbolPercent,             "symbol_percent"sv              },
			{ Token::Type::SymbolPipe,               "symbol_pipe"sv                },
			{ Token::Type::SymbolPipePipe,           "symbol_pipe_pipe"sv           },
			{ Token::Type::SymbolPlus,               "symbol_plus"sv                },
			{ Token::Type::SymbolPlusEqual,          "symbol_plus_equal"sv          },
			{ Token::Type::SymbolPlusPlus,           "symbol_plus_plus"sv           },
			{ Token::Type::SymbolQuestionMark,       "symbol_question_mark"sv       },
			{ Token::Type::SymbolSlash,              "symbol_slash"sv               },
			{ Token::Type::SymbolSlashEqual,         "symbol_slash_equal"sv         },
			{ Token::Type::SymbolStar,               "symbol_star"sv                },
			{ Token::Type::SymbolStarEqual,          "symbol_star_equal"sv          },
			{ Token::Type::SymbolBang,               "symbol_bang"sv                },
			{ Token::Type::SymbolBangEqual,          "symbol_bang_equal"sv          },
			{ Token::Type::SymbolBraceLeft,          "symbol_brace_left"sv          },
			{ Token::Type::SymbolBraceRight,         "symbol_brace_right"sv         },
			{ Token::Type::SymbolBracketLeft,        "symbol_bracket_left"sv        },
			{ Token::Type::SymbolBracketRight,       "symbol_bracket_right"sv       },
			{ Token::Type::SymbolColon,              "symbol_colon"sv               },
			{ Token::Type::SymbolColonColon,         "symbol_colon_colon"sv         },
			{ Token::Type::SymbolEqual,              "symbol_equal"sv               },
			{ Token::Type::SymbolEqualEqual,         "symbol_equal_equal"sv         },
			{ Token::Type::SymbolParenLeft,          "symbol_paren_left"sv          },
			{ Token::Type::SymbolParenRight,         "symbol_paren_right"sv         },
			{ Token::Type::SymbolSemicolon,          "symbol_semicolon"sv           },
			{ Token::Type::SpecialError,             "special_error"sv              },
			{ Token::Type::SpecialEndOfFile,         "special_eof"sv                },
		};
		return k_token_name.at(type);
	}
}  // namespace soul
