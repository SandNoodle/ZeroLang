#include "common/diagnostic.h"

#include <sstream>
#include <unordered_map>

namespace soul
{
	std::string_view get_base_diagnostic_message(DiagnosticCode code)
	{
		static const std::unordered_map<DiagnosticCode, std::string_view> k_messages = {
			{ DiagnosticCode::LexerUnrecognizedToken,    "unrecognized token"                       },
			{ DiagnosticCode::LexerValueIsNotANumber,    "value is not a number"                    },
			{ DiagnosticCode::LexerValueOutOfRange,      "value is out of range"                    },
			{ DiagnosticCode::LexerUnterminatedString,   "unterminated string"                      },
			{ DiagnosticCode::ParserOutOfRange,          "cannot peek the next token: out of range" },
			{ DiagnosticCode::TypeResolverTypeUnknown,   "unknown type '{}'"                        },
			{ DiagnosticCode::TypeResolverTypeRedefined, "redefinition of type '{}'"                },
			{ DiagnosticCode::TypeResolverTypeRedefined, "cannot infer the unknown type"            },
		};
		return k_messages.at(code);
	}

	[[nodiscard]] DiagnosticCode Diagnostic::code() const { return this->_code; }

	[[nodiscard]] std::string Diagnostic::message() const
	{
		static const std::unordered_map<DiagnosticType, char> k_short_types = {
			{ DiagnosticType::Hint,    'H' },
			{ DiagnosticType::Warning, 'W' },
			{ DiagnosticType::Error,   'E' },
		};

		std::stringstream ss;
		ss << "[" << k_short_types.at(_type);
		ss << std::format("{:04}]: ", static_cast<std::underlying_type_t<DiagnosticCode>>(_code));
		ss << _message << '\n';
		return ss.str();
	}

	Diagnostic::operator std::string() const { return message(); }

}  // namespace soul
