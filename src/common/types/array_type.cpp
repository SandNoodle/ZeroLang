#include "common/types/array_type.h"

#include "common/types/type.h"

namespace soul::types
{
	ArrayType::ArrayType(const Type& contained_type) : _type(std::make_unique<Type>(contained_type)) {}

	ArrayType::ArrayType(const ArrayType& other) noexcept : _type(std::make_unique<Type>(*other._type.get())) {}

	ArrayType::ArrayType(ArrayType&& other) noexcept : _type(std::make_unique<Type>(*other._type.get())) {}

	ArrayType& ArrayType::operator=(const ArrayType& other) noexcept
	{
		if (this == &other) {
			return *this;
		}
		_type = std::make_unique<Type>(*other._type.get());
		return *this;
	}

	ArrayType& ArrayType::operator=(ArrayType&& other) noexcept
	{
		if (this == &other) {
			return *this;
		}
		*_type = std::move(*other._type.get());
		return *this;
	}

	std::weak_ordering ArrayType::operator<=>(const ArrayType& other) const { return *_type <=> *other._type; }

	ArrayType::operator std::string() const { return std::string(*_type) + "[]"; }

	const Type& ArrayType::data_type() const noexcept { return *_type; }

	Type& ArrayType::data_type() noexcept { return *_type; }
}  // namespace soul::types
