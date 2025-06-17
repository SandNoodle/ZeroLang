#pragma once

#include "core/types.h"

#include <concepts>
#include <string>
#include <type_traits>
#include <variant>

namespace soul::types
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
			Boolean,
			Char,
			Float32,
			Float64,
			Int32,
			Int64,
			String,
			Void,
		};

		public:
		Kind type = Kind::Unknown;

		public:
		PrimitiveType(Kind type = Kind::Unknown) : type(type) {}

		bool               operator==(const PrimitiveType&) const noexcept  = default;
		std::weak_ordering operator<=>(const PrimitiveType&) const = default;
		explicit operator std::string() const;
	};
}  // namespace soul::types
