#pragma once

#include "ast/ast.h"
#include "core/types.h"

#include <concepts>
#include <sstream>
#include <string>
#include <type_traits>
#include <variant>

namespace soul
{
	template <typename T>
	concept ValueKind = std::same_as<T, std::monostate>  //
	                 || std::same_as<T, bool>            //
	                 || std::same_as<T, i64>             //
	                 || std::same_as<T, f64>             //
	                 || std::same_as<T, std::string>     //
	                 || std::same_as<T, char>            //
		;

	/**
	 * @brief
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
		explicit              operator std::string() const noexcept;

		/**
		 * @brief
		 */
		template <ValueKind T>
		[[nodiscard]] constexpr bool is() const noexcept
		{
			return std::holds_alternative<T>(value);
		}

		/**
		 * @brief
		 */
		template <ValueKind T>
		[[nodiscard]] constexpr const T& get() const noexcept
		{
			return std::get<T>(value);
		}

		template <ValueKind T>
		[[nodiscard]] constexpr T& get() noexcept
		{
			return std::get<T>(value);
		}
	};

}  // namespace soul
