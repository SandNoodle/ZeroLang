#pragma once

#include <cstdint>
#include <type_traits>
#include <variant>
#include <string>
#include <string_view>

namespace soul
{
	enum class token_type_t : uint8_t
	{
		token_unknown,

		// Single character tokens
		token_semicolon, token_question_mark,    // ;?
		token_percent, token_caret,              // %^
		token_dot, token_comma,                  // .,
		token_paren_left, token_paren_right,     // ()
		token_brace_left, token_brace_right,     // {}
		token_bracket_left, token_bracket_right, // []

		// One or two character tokens
		token_colon, token_double_colon,                    // : ::
		token_equal, token_double_equal,                    // = ==
		token_bang, token_bang_equal,                       // ! !=
		token_greater, token_greater_equal,                 // > >=
		token_less, token_less_equal,                       // < <=
		token_plus, token_plus_equal, token_double_plus,    // + += ++
		token_minus, token_minus_equal, token_double_minus, // - -= --
		token_star, token_star_equal,                       // * *=
		token_slash, token_slash_equal,                     // / /=
		token_ampersand, token_double_ampersand,            // & &&
		token_pipe, token_double_pipe,                      // | ||

		// Literals
		token_literal_integer,    // 0, 1, 2, ...
		token_literal_float,      // 3.14, 5.72, ...
		token_literal_string,     // "my_string", ...
		token_literal_identifier, // my_variable, ...

		// Keywords
		token_native,
		token_let,
		token_mut,
		token_if,
		token_else,
		token_for,
		token_while,
		token_continue,
		token_break,
		token_return,
		token_fn,
		token_struct,
		token_true,
		token_false,

		// Special tokens
		token_error, // Token containing error message.
		token_eof,   // End of File.
	};

	/**
	 * @brief
	 */
	std::string_view token_type_to_string(token_type_t type);

	template <typename T>
	T& operator<<(T& stream, const token_type_t& token)
	{
		stream << token_type_to_string(token);
		return stream;
	}

	template <typename T>
	concept is_value_t = std::same_as<T, int64_t>         //
	                  || std::same_as<T, double>          //
	                  || std::same_as<T, std::string>     //
	                  || std::same_as<T, std::monostate>  //
	;

	/**
	 * @brief Class representing a single lexical token.
	 * Otherwise known as a 'Lexeme'.
	 */
	class token_t
	{
		public:
			using empty_t = std::monostate;
			using value_t = std::variant<empty_t, int64_t, double, std::string>;

		private:
			token_type_t _type  = token_type_t::token_unknown;
			value_t      _value = empty_t{};

		public:
			explicit token_t(token_type_t type, const value_t& value = empty_t{});

			bool operator==(const token_t&) const noexcept = default;
			auto operator<=>(const token_t&) const noexcept = default;
			explicit operator std::string() const;

			/** @brief Returns the token's type. */
			constexpr token_type_t type() const
			{
				return _type;
			}

			/**
			 * @brief Verifies if the token is of a specific value type.
			 * @tparam T Type of value to verify.
			 * @return true if it is, false otherwise.
			 */
			template <is_value_t T>
			constexpr bool has() const
			{
				return std::holds_alternative<T>(_value);
			}

			/**
			 * @brief Returns the value that token holds.
			 * @tparam T Type of value to return.
			 * @return Value held in a token of type T.
			 */
			template <is_value_t T>
			constexpr const T& get() const
			{
				return std::get<T>(_value);
			}

	};

	template <typename T>
	T& operator<<(T& stream, const token_t& token)
	{
		stream << std::string(token);
		return stream;
	}

}  // namespace soul
