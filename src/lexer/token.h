#pragma once

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

	template <typename T>
	concept ValueKind = std::same_as<T, i64>             //
	                 || std::same_as<T, f64>             //
	                 || std::same_as<T, std::string>     //
	                 || std::same_as<T, std::monostate>  //
		;

	/**
	 * @brief Class representing a single lexical token.
	 * Otherwise known as a 'Lexeme'.
	 */
	class Token
	{
		public:
		using ValueType = std::variant<std::monostate, i64, f64, std::string>;

		private:
		TokenType _type  = TokenType::Unknown;
		ValueType _value = std::monostate{};

		public:
		explicit Token(TokenType type, ValueType value = std::monostate{});

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
		 * @brief Verifies if the token is of a specific value type.
		 * @tparam T Type of value to verify.
		 * @return true if it is, false otherwise.
		 */
		template <ValueKind T>
		[[nodiscard]] constexpr bool has() const
		{
			return std::holds_alternative<T>(_value);
		}

		/**
		 * @brief Verifies that token holds no value.
		 * @return true if token does not contain a value, false otherwise.
		 */
		[[nodiscard]] constexpr bool no_value() const { return std::holds_alternative<std::monostate>(_value); }

		/**
		 * @brief Returns the value that token holds.
		 * @tparam T Type of value to return.
		 * @return Value held in a token of type T.
		 */
		template <ValueKind T>
		[[nodiscard]] constexpr const T& get() const
		{
			return std::get<T>(_value);
		}

		/**
		 * @brief Returns the value that token holds.
		 * @tparam T Type of value to return.
		 * @return Value held in a token of type T.
		 */
		template <ValueKind T>
		[[nodiscard]] constexpr T& get()
		{
			return std::get<T>(_value);
		}

		[[nodiscard]] constexpr const ValueType& value() const noexcept { return _value; }
		[[nodiscard]] constexpr ValueType&       value() noexcept { return _value; }
	};

	template <typename T>
	T& operator<<(T& stream, const Token& token)
	{
		stream << static_cast<std::string>(token);
		return stream;
	}

}  // namespace soul
