#pragma once

#include "core/types.h"

#include <format>
#include <string>
#include <string_view>
#include <vector>

namespace soul
{
	/**
	 * @brief Diagnostic code emitted by the Lexer, Parser, Compiler etc. in the language.
	 * @detail Created in the following format <type>_<object>_<diagnostic>, where:
	 * - <type> is either hint, warning or error.
	 * - <object> is either lexer, parser, compiler or type_checker.
	 * - <diagnostic> is an unique diagnostic value.
	 */
	enum class DiagnosticCode : u8
	{
		ErrorLexerUnrecognizedToken,
		ErrorLexerValueIsNotANumber,
		ErrorLexerValueOutOfRange,
		ErrorLexerUnterminatedString,
		ErrorParserOutOfRange,
	};

	/** * @brief Returns the base, un-formatted diagnostic message. */
	std::string_view get_base_diagnostic_message(DiagnosticCode);

	class Diagnostic
	{
		public:
		private:
		DiagnosticCode _code;
		std::string    _message;

		public:
		Diagnostic()                           = delete;
		Diagnostic(const Diagnostic&) noexcept = default;
		Diagnostic(Diagnostic&&) noexcept      = default;

		template <typename... Args>
		explicit Diagnostic(DiagnosticCode code, Args&&... args);

		Diagnostic& operator=(const Diagnostic&) noexcept = default;
		Diagnostic& operator=(Diagnostic&&) noexcept      = default;

		[[nodiscard]] DiagnosticCode code() const;

		/**
		 * @brief Constructs human readable diagnostic message.
		 */
		[[nodiscard]] std::string message() const;

		explicit operator std::string() const;
	};

	template <typename... Args>
	Diagnostic::Diagnostic(DiagnosticCode code, Args&&... args) : _code(code)
	{
		std::string_view message = get_base_diagnostic_message(code);
		this->_message           = std::vformat(message, std::make_format_args(std::forward<Args>(args)...));
	}

	using Diagnostics = std::vector<Diagnostic>;
}  // namespace soul
