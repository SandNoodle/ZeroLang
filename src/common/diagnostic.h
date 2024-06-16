#pragma once

#include <cstdint>
#include <string>
#include <format>
#include <unordered_map>

namespace soul
{
	/**
	 * @brief Diagnostic code emitted by the Lexer, Parser, Compiler etc. in the language.
	 * @detail Created in the following format <type>_<object>_<diagnostic>, where:
	 * - <type> is either hint, warning or error.
	 * - <object> is either lexer, parser, compiler or type_checker.
	 * - <diagnostic> is an unique diagnostic value.
	 */
	enum class diagnostic_code_t : uint8_t
	{
		error_lexer_out_of_range,
		error_lexer_unrecognized_token,
		error_lexer_value_is_not_a_number,
		error_lexer_value_out_of_range,
		error_lexer_unterminated_string,
		error_parser_out_of_range,
	};

	class diagnostic_t
	{
		public:

		private:
			diagnostic_code_t _code;
			std::string _message;

		public:
			diagnostic_t() = delete;
			diagnostic_t(const diagnostic_t&) noexcept = default;
			diagnostic_t(diagnostic_t&&) noexcept = default;

			template <typename... Args>
			explicit diagnostic_t(diagnostic_code_t code, Args&&... args);

			diagnostic_t& operator=(const diagnostic_t&) noexcept = default;
			diagnostic_t& operator=(diagnostic_t&&) noexcept = default;

			explicit operator std::string() const;
	};

	template<typename... Args>
	diagnostic_t::diagnostic_t(diagnostic_code_t code, Args&&... args)
		: _code(code)
	{
		static const std::unordered_map<diagnostic_code_t, std::string_view> k_messages = {
				{ diagnostic_code_t::error_lexer_out_of_range, "cannot get the next character: out of range" },
				{ diagnostic_code_t::error_lexer_unrecognized_token, "unrecognized token" },
				{ diagnostic_code_t::error_lexer_value_is_not_a_number, "value is not a number" },
				{ diagnostic_code_t::error_lexer_value_out_of_range, "value is out of range" },
				{ diagnostic_code_t::error_lexer_unterminated_string, "unterminated string" },
				{ diagnostic_code_t::error_parser_out_of_range, "cannot peek the next token: out of range" },
		};
		this->_message = std::vformat(k_messages.at(code), std::make_format_args(std::forward<Args>(args)...));
	}
}  // namespace soul
