#pragma once

#include "common/types/array_type.h"
#include "common/types/primitive_type.h"
#include "common/types/struct_type.h"

#include <concepts>
#include <string>
#include <type_traits>
#include <variant>

namespace soul::types
{
	/**
	 * @brief TypeKind is a concept that specifies (and is satisfied by) types in the language's type system that
	 * it supports.
	 */
	template <typename T>
	concept TypeKind = std::same_as<T, PrimitiveType>  //
	                || std::same_as<T, ArrayType>      //
	                || std::same_as<T, StructType>     //
		;

	std::weak_ordering operator<=>(const Type& lhs, const Type& rhs);

	/**
	 * @brief Class that represents a specific type in the language's type system.
	 * It is capable of describing all builtin, nested and user defined types.
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

		Type&    operator=(const Type&) noexcept        = default;
		Type&    operator=(Type&&) noexcept             = default;
		bool     operator==(const Type&) const noexcept = default;
		explicit operator std::string() const;

		/**
		 * @brief Verifies if a Type is of a given TypeKind's type.
		 * @tparam T Type satisfying the TypeKind concept.
		 * @return \b true if it is, \b false otherwise.
		 */
		template <TypeKind T>
		[[nodiscard]] constexpr bool is() const noexcept
		{
			return std::holds_alternative<T>(_type);
		}

		/**
		 * @brief Returns the requested underlying type.
		 * @important Does not perform any validation - assumes that Type::is<T> was used first.
		 * @tparam T Type satisfying the TypeKind concept.
		 */
		template <TypeKind T>
		[[nodiscard]] constexpr const T& as() const noexcept
		{
			return std::get<T>(_type);
		}

		friend std::ostream&      operator<<(std::ostream& os, const Type& type);
		friend std::weak_ordering operator<=>(const Type&, const Type&);
	};
}  // namespace soul::types
