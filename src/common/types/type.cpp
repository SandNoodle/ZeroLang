#include "common/types/type.h"

namespace soul::types
{
	Type::operator std::string() const
	{
		return std::visit([](const auto& arg) -> std::string { return std::string(arg); }, _type);
	}

	std::ostream& operator<<(std::ostream& os, const Type& type) { return os << std::string(type); }
}  // namespace soul::types
