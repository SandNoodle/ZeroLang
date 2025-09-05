#pragma once

#include "common/source_location.h"
#include "core/types.h"

#include <format>
#include <string>

namespace soul
{
	/**
	 * @brief Class representing a single lexical token.
	 * Otherwise known as a 'Lexeme'.
	 */
	struct Token
	{
		public:
		enum class Type : u8
		{
			KeywordBreak,
			KeywordCast,
			KeywordContinue,
			KeywordElse,
			KeywordFalse,
			KeywordFn,
			KeywordFor,
			KeywordIf,
			KeywordLet,
			KeywordMut,
			KeywordNative,
			KeywordReturn,
			KeywordStruct,
			KeywordTrue,
			KeywordWhile,

			LiteralFloat,
			LiteralIdentifier,
			LiteralInteger,
			LiteralString,

			SymbolAmpersand,
			SymbolAmpersandAmpersand,
			SymbolBang,
			SymbolBangEqual,
			SymbolBraceLeft,
			SymbolBraceRight,
			SymbolBracketLeft,
			SymbolBracketRight,
			SymbolCaret,
			SymbolColon,
			SymbolColonColon,
			SymbolComma,
			SymbolDot,
			SymbolEqual,
			SymbolEqualEqual,
			SymbolGreater,
			SymbolGreaterEqual,
			SymbolLess,
			SymbolLessEqual,
			SymbolMinus,
			SymbolMinusEqual,
			SymbolMinusMinus,
			SymbolPercent,
			SymbolParenLeft,
			SymbolParenRight,
			SymbolPipe,
			SymbolPipePipe,
			SymbolPlus,
			SymbolPlusEqual,
			SymbolPlusPlus,
			SymbolQuestionMark,
			SymbolSemicolon,
			SymbolSlash,
			SymbolSlashEqual,
			SymbolStar,
			SymbolStarEqual,

			SpecialError,
			SpecialEndOfFile,
		} type;
		std::string_view data;
		SourceLocation   location;

		constexpr bool operator==(const Token& other) const noexcept
		{
			return std::tie(type, data) == std::tie(other.type, other.data);
		}

		constexpr std::strong_ordering operator<=>(const Token& other) const noexcept
		{
			return std::tie(type, data) <=> std::tie(other.type, other.data);
		}

		explicit constexpr operator std::string() const
		{
			return std::format("<{}:\"{}\">", internal_name(type), data);
		}

		static std::string_view name(Token::Type type) noexcept;
		static std::string_view internal_name(Token::Type type) noexcept;
	};

	template <typename T>
	constexpr T& operator<<(T& stream, const Token& token)
	{
		stream << static_cast<std::string>(token);
		return stream;
	}
}  // namespace soul
