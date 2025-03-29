#pragma once

#include "core/types.h"

#include <concepts>
#include <string>
#include <type_traits>
#include <variant>

namespace soul::ast::types
{
	/**
	 * @brief
	 */
	class PrimitiveType
	{
		public:
		enum class Kind : u8
		{
			Unknown,
			Void,
			Int32,
			Int64,
			Float32,
			Float64,
			String,
			Char,
		};

		public:
		Kind type = Kind::Unknown;

		public:
		PrimitiveType(Kind type = Kind::Unknown) : type(type) {}

		bool               operator==(const PrimitiveType&) const noexcept  = default;
		std::weak_ordering operator<=>(const PrimitiveType&) const noexcept = default;

		/**
		 * @brief TODO
		 * @param expected_type
		 * @return Returns true if it is, false otherwise.
		 */
		[[nodiscard]] constexpr bool is(Kind expected_type) const noexcept { return type == expected_type; }
	};
}  // namespace soul::ast::types
