#include "common/diagnostic.h"

#include <sstream>
#include <unordered_map>

namespace soul
{
	std::string_view get_base_diagnostic_message(diagnostic_code_t code)
	{
		static const std::unordered_map<diagnostic_code_t, std::string_view> k_messages = {
				{ diagnostic_code_t::error_lexer_unrecognized_token, "unrecognized token" },
				{ diagnostic_code_t::error_lexer_value_is_not_a_number, "value is not a number" },
				{ diagnostic_code_t::error_lexer_value_out_of_range, "value is out of range" },
				{ diagnostic_code_t::error_lexer_unterminated_string, "unterminated string" },
				{ diagnostic_code_t::error_parser_out_of_range, "cannot peek the next token: out of range" },
		};
		return k_messages.at(code);
	}

	[[nodiscard]] diagnostic_code_t diagnostic_t::code() const
	{
		return this->_code;
	}

	std::string diagnostic_t::message() const
	{
		std::stringstream ss;
		ss << "ERROR"; // TODO: Other types
		ss <<  std::format(" [E{:04}", static_cast<std::underlying_type_t<diagnostic_code_t>>(_code)) << "]: ";
		ss << _message << '\n';
		return ss.str();
	}

	diagnostic_t::operator std::string() const
	{
		return "unimplemented.";
	}

}  // namespace soul
