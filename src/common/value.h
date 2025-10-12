#pragma once

#include "core/types.h"

#include <concepts>
#include <sstream>
#include <string>
#include <type_traits>
#include <variant>

namespace soul
{
	/**
	 * @brief ValueKind is a concept that specifies (and is satisfied by) types that can be represented in the language.
	 */
	template <typename T>
	concept ValueKind = std::same_as<T, std::monostate>  //
	                 || std::same_as<T, bool>            //
	                 || std::same_as<T, i64>             //
	                 || std::same_as<T, f64>             //
	                 || std::same_as<T, std::string>     //
	                 || std::same_as<T, char>            //
		;

	/**
	 * @brief Represents a single `value` of a given type in the language.
	 */
	class Value
	{
		public:
		using UnknownValue = std::monostate;
		using Variant      = std::variant<UnknownValue, bool, i64, f64, std::string, char>;

		public:
		Variant value = UnknownValue{};

		public:
		Value()                      = default;
		Value(const Value&) noexcept = default;
		Value(Value&&) noexcept      = default;
		Value(Variant value);

		Value&                operator=(const Value&) noexcept        = default;
		Value&                operator=(Value&&) noexcept             = default;
		bool                  operator==(const Value&) const noexcept = default;
		std::partial_ordering operator<=>(const Value& other) const noexcept;
		explicit              operator std::string() const;

		/**
		 * @brief Verifies if Value of a given ValueKind type.
		 * @tparam T Type satisfying the ValueKind concept.
		 * @return \b true if it is, \b false otherwise.
		 */
		template <ValueKind T>
		[[nodiscard]] constexpr bool is() const noexcept
		{
			return std::holds_alternative<T>(value);
		}

		/**
		 * @brief Returns the underlying value (of a given type).
		 * @important Does not perform any validation - assumes that Value::is<T> was used first.
		 * @tparam T Type satisfying the ValueKind concept.
		 */
		template <ValueKind T>
		[[nodiscard]] constexpr const T& get() const noexcept
		{
			return std::get<T>(value);
		}

		/**
		 * @brief Returns the underlying value (of a given type).
		 * @important Does not perform any validation - assumes that Value::is<T> was used first.
		 * @tparam T Type satisfying the ValueKind concept.
		 */
		template <ValueKind T>
		[[nodiscard]] constexpr T& get() noexcept
		{
			return std::get<T>(value);
		}
	};

}  // namespace soul
