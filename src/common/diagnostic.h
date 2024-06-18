#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <format>

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
		error_lexer_unrecognized_token,
		error_lexer_value_is_not_a_number,
		error_lexer_value_out_of_range,
		error_lexer_unterminated_string,
		error_parser_out_of_range,
	};

	/** * @brief Returns the base, un-formatted diagnostic message. */
	std::string_view get_base_diagnostic_message(diagnostic_code_t);

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

			[[nodiscard]] diagnostic_code_t code() const;

			/**
			 * @brief Constructs human readable diagnostic message.
			 */
			std::string message() const;

			explicit operator std::string() const;
	};

	template<typename... Args>
	diagnostic_t::diagnostic_t(diagnostic_code_t code, Args&&... args)
		: _code(code)
	{
		std::string_view message = get_base_diagnostic_message(code);
		this->_message = std::vformat(message, std::make_format_args(std::forward<Args>(args)...));
	}
}  // namespace soul
