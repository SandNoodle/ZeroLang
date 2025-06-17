#pragma once

#include "core/types.h"

#include <format>
#include <string>
#include <string_view>
#include <vector>

namespace soul
{
	enum class DiagnosticType : u8
	{
		Hint,
		Warning,
		Error,
	};

	enum class DiagnosticCode : u8
	{
		LexerUnrecognizedToken,
		LexerValueIsNotANumber,
		LexerValueOutOfRange,
		LexerUnterminatedString,
		ParserOutOfRange,
		TypeResolverTypeUnknown,
		TypeResolverTypeRedefined,
		TypeResolverCannotInferUnknownType,
		TypeResolverForceStrictCasts,
	};

	/** * @brief Returns the base, un-formatted diagnostic message. */
	std::string_view get_base_diagnostic_message(DiagnosticCode);

	class Diagnostic
	{
		public:
		private:
		DiagnosticType _type;
		DiagnosticCode _code;
		std::string    _message;

		public:
		Diagnostic()                           = delete;
		Diagnostic(const Diagnostic&) noexcept = default;
		Diagnostic(Diagnostic&&) noexcept      = default;

		template <typename... Args>
		explicit Diagnostic(DiagnosticType type, DiagnosticCode code, Args&&... args);

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
	Diagnostic::Diagnostic(DiagnosticType type, DiagnosticCode code, Args&&... args) : _type(type), _code(code)

	{
		_message = std::vformat(get_base_diagnostic_message(code), std::make_format_args(std::forward<Args>(args)...));
	}

	using Diagnostics = std::vector<Diagnostic>;
}  // namespace soul
