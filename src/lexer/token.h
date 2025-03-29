#pragma once

#include "common/value.h"
#include "core/types.h"

#include <string>
#include <string_view>
#include <type_traits>
#include <variant>

namespace soul
{
	enum class TokenType : u8
	{
		// clang-format off
		Unknown,

		// Single character tokens
		Semicolon, QuestionMark,   // ;?
		Percent, Caret,            // %^
		Dot, Comma,                // .,
		ParenLeft, ParenRight,     // ()
		BraceLeft, BraceRight,     // {}
		BracketLeft, BracketRight, // []

		// One or two character tokens
		Colon, DoubleColon,             // : ::
		Equal, DoubleEqual,             // = ==
		Bang, BangEqual,                // ! !=
		Greater, GreaterEqual,          // > >=
		Less, LessEqual,                // < <=
		Plus, PlusEqual, DoublePlus,    // + += ++
		Minus, MinusEqual, DoubleMinus, // - -= --
		Star, StarEqual,                // * *=
		Slash, SlashEqual,              // / /=
		Ampersand, DoubleAmpersand,     // & &&
		Pipe, DoublePipe,               // | ||

		// Literals
		LiteralInteger,    // 0, 1, 2, ...
		LiteralFloat,      // 3.14, 5.72, ...
		LiteralString,     // "my_string", ...
		LiteralIdentifier, // my_variable, ...

		// Keywords
		KeywordBreak,
		KeywordCast,
		KeywordContinue,
		KeywordElse,
		KeywordFalse,
		KeywordFn,
		KeywordFor,
		KeywordForeach,
		KeywordIf,
		KeywordIn,
		KeywordLet,
		KeywordMut,
		KeywordNative,
		KeywordReturn,
		KeywordStruct,
		KeywordTrue,
		KeywordWhile,

		// Special tokens
		EndOfFile,   // End of File.
		            // clang-format on
	};

	/** @brief Stringifies the token type. */
	std::string_view to_string(TokenType type);

	template <typename T>
	T& operator<<(T& stream, const TokenType& token)
	{
		stream << to_string(token);
		return stream;
	}

	/**
	 * @brief Class representing a single lexical token.
	 * Otherwise known as a 'Lexeme'.
	 */
	class Token
	{
		private:
		TokenType _type = TokenType::Unknown;

		public:
		Value value = {};

		public:
		explicit Token(TokenType type, Value value = {});

		bool     operator==(const Token&) const noexcept  = default;
		auto     operator<=>(const Token&) const noexcept = default;
		explicit operator std::string() const;

		[[nodiscard]] TokenType type() const;
		[[nodiscard]] bool      is_type(const TokenType type) const noexcept;
		[[nodiscard]] bool      is_one_of_type(const TokenType t1, const TokenType t2) const noexcept;
		template <typename... Types>
		[[nodiscard]] bool is_one_of_type(const TokenType t1, Types... types) const noexcept
		{
			return _type == t1 || isOneOfTypes(std::forward<Types>(types)...);
		}

		/**
		 * @brief Verifies that token holds no value.
		 * @return true if token does not contain a value, false otherwise.
		 */
		[[nodiscard]] constexpr bool no_value() const { return value.is<Value::UnknownValue>(); }
	};

	template <typename T>
	T& operator<<(T& stream, const Token& token)
	{
		stream << static_cast<std::string>(token);
		return stream;
	}
}  // namespace soul
