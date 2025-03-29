#pragma once

#include "core/types.h"

#include <memory>

namespace soul::ast::types
{
	class Type;

	/**
	 * @brief
	 * @detail
	 */
	class ArrayType
	{
		private:
		std::unique_ptr<Type> _type;

		public:
		ArrayType(const Type& contained_type);
		ArrayType(const ArrayType&) noexcept;
		ArrayType(ArrayType&&) noexcept;

		ArrayType& operator=(const ArrayType&) noexcept;
		ArrayType& operator=(ArrayType&&) noexcept;
		bool       operator==(const ArrayType&) const noexcept = default;
		std::weak_ordering operator<=>(const ArrayType&) const noexcept;

		const Type& type() const noexcept;
		Type&       type() noexcept;
	};

}  // namespace soul::ast::types
