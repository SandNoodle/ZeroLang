#include "common/diagnostic.h"

#include <sstream>
#include <unordered_map>

namespace soul
{
	std::string_view get_base_diagnostic_message(DiagnosticCode code)
	{
		static const std::unordered_map<DiagnosticCode, std::string_view> k_messages = {
			{ DiagnosticCode::ErrorLexerUnrecognizedToken,    "unrecognized token"                       },
			{ DiagnosticCode::ErrorLexerValueIsNotANumber, "value is not a number"                    },
			{ DiagnosticCode::ErrorLexerValueOutOfRange,    "value is out of range"                    },
			{ DiagnosticCode::ErrorLexerUnterminatedString,   "unterminated string"                      },
			{ DiagnosticCode::ErrorParserOutOfRange,         "cannot peek the next token: out of range" },
		};
		return k_messages.at(code);
	}

	[[nodiscard]] DiagnosticCode Diagnostic::code() const { return this->_code; }

	[[nodiscard]] std::string Diagnostic::message() const
	{
		std::stringstream ss;
		ss << "ERROR";  // TODO: Other types
		ss << std::format(" [E{:04}", static_cast<std::underlying_type_t<DiagnosticCode>>(_code)) << "]: ";
		ss << _message << '\n';
		return ss.str();
	}

	Diagnostic::operator std::string() const { return "unimplemented."; }

}  // namespace soul
