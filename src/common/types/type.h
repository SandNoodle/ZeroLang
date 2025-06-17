#pragma once

#include "common/types/array_type.h"
#include "common/types/primitive_type.h"
#include "common/types/struct_type.h"

#include <concepts>
#include <type_traits>
#include <variant>
#include <string>

namespace soul::types
{
	/**
	 * @brief Concept defining all the possible Type System's builtin types.
	 */
	template <typename T>
	concept TypeKind = std::same_as<T, PrimitiveType>  //
	                || std::same_as<T, ArrayType>      //
	                || std::same_as<T, StructType>     //
		;

	std::weak_ordering operator<=>(const Type& lhs, const Type& rhs);

	/**
	 * @brief Backbone of the Soul's Type System.
	 * This class is capable of representing all possible builtin and user defined types.
	 */
	class Type
	{
		public:
		using Variant = std::variant<PrimitiveType, ArrayType, StructType>;

		private:
		Variant _type = PrimitiveType::Kind::Unknown;

		public:
		Type() noexcept            = default;
		Type(const Type&) noexcept = default;
		Type(Type&&) noexcept      = default;
		explicit Type(Variant&& type) noexcept;
		Type(PrimitiveType::Kind primitive_type);

		Type& operator=(const Type&) noexcept        = default;
		Type& operator=(Type&&) noexcept             = default;
		bool  operator==(const Type&) const noexcept = default;
		explicit operator std::string() const;

		/**
		 * @brief
		 * @tparam
		 * @return
		 */
		template <TypeKind T>
		[[nodiscard]] constexpr bool is() const noexcept
		{
			return std::holds_alternative<T>(_type);
		}

		template <TypeKind T>
		[[nodiscard]] constexpr const T& as() const noexcept
		{
			return std::get<T>(_type);
		}

		friend std::weak_ordering operator<=>(const Type&, const Type&);
	};
}  // namespace soul::types
