#include "common/diagnostic.h"

#include <sstream>
#include <unordered_map>

namespace soul
{
	std::string_view get_base_diagnostic_message(DiagnosticCode code)
	{
		static const std::unordered_map<DiagnosticCode, std::string_view> k_messages = {
				{DiagnosticCode::ERROR_LEXER_UNRECOGNIZED_TOKEN,    "unrecognized token" },
				{DiagnosticCode::ERROR_LEXER_VALUE_IS_NOT_A_NUMBER, "value is not a number" },
				{DiagnosticCode::ERROR_LEXER_VALUE_OUT_OF_RANGE,    "value is out of range" },
				{DiagnosticCode::ERROR_LEXER_UNTERMINATED_STRING,   "unterminated string" },
				{DiagnosticCode::ERROR_PARSER_OUT_OF_RANGE,         "cannot peek the next token: out of range" },
		};
		return k_messages.at(code);
	}

	[[nodiscard]] DiagnosticCode Diagnostic::code() const
	{
		return this->_code;
	}

	[[nodiscard]] std::string Diagnostic::message() const
	{
		std::stringstream ss;
		ss << "ERROR"; // TODO: Other types
		ss << std::format(" [E{:04}", static_cast<std::underlying_type_t<DiagnosticCode>>(_code)) << "]: ";
		ss << _message << '\n';
		return ss.str();
	}

	Diagnostic::operator std::string() const
	{
		return "unimplemented.";
	}

}  // namespace soul
