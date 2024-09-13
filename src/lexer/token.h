#pragma once

#include "common/types.h"

#include <string>
#include <string_view>
#include <type_traits>
#include <variant>

namespace soul
{
	enum class TokenType : u8
	{
		// clang-format off
		TOKEN_UNKNOWN,

		// Single character tokens
		TOKEN_SEMICOLON, TOKEN_QUESTION_MARK,    // ;?
		TOKEN_PERCENT, TOKEN_CARET,              // %^
		TOKEN_DOT, TOKEN_COMMA,                  // .,
		TOKEN_PAREN_LEFT, TOKEN_PAREN_RIGHT,     // ()
		TOKEN_BRACE_LEFT, TOKEN_BRACE_RIGHT,     // {}
		TOKEN_BRACKET_LEFT, TOKEN_BRACKET_RIGHT, // []

		// One or two character tokens
		TOKEN_COLON, TOKEN_DOUBLE_COLON,                    // : ::
		TOKEN_EQUAL, TOKEN_DOUBLE_EQUAL,                    // = ==
		TOKEN_BANG, TOKEN_BANG_EQUAL,                       // ! !=
		TOKEN_GREATER, TOKEN_GREATER_EQUAL,                 // > >=
		TOKEN_LESS, TOKEN_LESS_EQUAL,                       // < <=
		TOKEN_PLUS, TOKEN_PLUS_EQUAL, TOKEN_DOUBLE_PLUS,    // + += ++
		TOKEN_MINUS, TOKEN_MINUS_EQUAL, TOKEN_DOUBLE_MINUS, // - -= --
		TOKEN_STAR, TOKEN_STAR_EQUAL,                       // * *=
		TOKEN_SLASH, TOKEN_SLASH_EQUAL,                     // / /=
		TOKEN_AMPERSAND, TOKEN_DOUBLE_AMPERSAND,            // & &&
		TOKEN_PIPE, TOKEN_DOUBLE_PIPE,                      // | ||

		// Literals
		TOKEN_LITERAL_INTEGER,    // 0, 1, 2, ...
		TOKEN_LITERAL_FLOAT,      // 3.14, 5.72, ...
		TOKEN_LITERAL_STRING,     // "my_string", ...
		TOKEN_LITERAL_IDENTIFIER, // my_variable, ...

		// Keywords
		TOKEN_BREAK,
		TOKEN_CONTINUE,
		TOKEN_ELSE,
		TOKEN_FALSE,
		TOKEN_FN,
		TOKEN_FOR,
		TOKEN_IF,
		TOKEN_LET,
		TOKEN_MUT,
		TOKEN_NATIVE,
		TOKEN_RETURN,
		TOKEN_STRUCT,
		TOKEN_TRUE,
		TOKEN_WHILE,

		// Special tokens
		TOKEN_EOF,   // End of File.
		            // clang-format on
	};

	/** @brief Stringifies the token type. */
	std::string_view token_type_to_string_view(TokenType type);

	template <typename T>
	T& operator<<(T& stream, const TokenType& token)
	{
		stream << token_type_to_string_view(token);
		return stream;
	}

	template <typename T>
	concept is_value_t = std::same_as<T, i64>             //
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
		using empty_t = std::monostate;
		using value_t = std::variant<empty_t, i64, f64, std::string>;

		private:
		TokenType _type  = TokenType::TOKEN_UNKNOWN;
		value_t   _value = empty_t{};

		public:
		explicit Token(TokenType type, value_t value = empty_t{});

		bool     operator==(const Token&) const noexcept  = default;
		auto     operator<=>(const Token&) const noexcept = default;
		explicit operator std::string() const;

		/** @brief Returns the token's type. */
		[[nodiscard]] constexpr TokenType type() const { return _type; }

		/**
		 * @brief Verifies if the token is of a specific value type.
		 * @tparam T Type of value to verify.
		 * @return true if it is, false otherwise.
		 */
		template <is_value_t T>
		[[nodiscard]] constexpr bool has() const
		{
			return std::holds_alternative<T>(_value);
		}

		/**
		 * @brief Verifies that token holds no value.
		 * @return true if token does not contain a value, false otherwise.
		 */
		[[nodiscard]] constexpr bool no_value() const { return std::holds_alternative<empty_t>(_value); }

		/**
		 * @brief Returns the value that token holds.
		 * @tparam T Type of value to return.
		 * @return Value held in a token of type T.
		 */
		template <is_value_t T>
		[[nodiscard]] constexpr const T& get() const
		{
			return std::get<T>(_value);
		}

		/**
		 * @brief Returns the value that token holds.
		 * @tparam T Type of value to return.
		 * @return Value held in a token of type T.
		 */
		template <is_value_t T>
		[[nodiscard]] constexpr T& get()
		{
			return std::get<T>(_value);
		}
	};

	template <typename T>
	T& operator<<(T& stream, const Token& token)
	{
		stream << std::string(token);
		return stream;
	}

}  // namespace soul
