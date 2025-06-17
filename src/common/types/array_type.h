#pragma once

#include "core/types.h"

#include <memory>

namespace soul::types
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

		ArrayType&         operator=(const ArrayType&) noexcept;
		ArrayType&         operator=(ArrayType&&) noexcept;
		bool               operator==(const ArrayType&) const noexcept = default;
		std::weak_ordering operator<=>(const ArrayType&) const;
		explicit operator std::string() const;

		const Type& data_type() const noexcept;
		Type&       data_type() noexcept;
	};

}  // namespace soul::types
