#include "ast/types/type.h"

namespace soul::ast::types
{
	std::weak_ordering operator<=>(const Type& lhs, const Type& rhs) noexcept
	{
		return std::tie(lhs._type) <=> std::tie(rhs._type);
	}

	Type::Type(Variant&& type) noexcept : _type(std::move(type)) {}
	Type::Type(PrimitiveType::Kind type) : _type(PrimitiveType(type)) {}
}  // namespace soul::ast::types
