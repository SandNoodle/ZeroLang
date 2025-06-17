#include "common/types/type.h"

namespace soul::types
{
	std::weak_ordering operator<=>(const Type& lhs, const Type& rhs)
	{
		return std::tie(lhs._type) <=> std::tie(rhs._type);
	}

	Type::Type(Variant&& type) noexcept : _type(std::move(type)) {}
	Type::Type(PrimitiveType::Kind type) : _type(PrimitiveType(type)) {}

	Type::operator std::string() const
	{
		return std::visit([](const auto& arg) -> std::string { return std::string(arg); }, _type);
	}
}  // namespace soul::types
