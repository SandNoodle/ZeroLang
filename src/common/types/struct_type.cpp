#include "common/types/struct_type.h"

#include "common/types/type.h"

#include <algorithm>
#include <expected>
#include <sstream>

namespace soul::types
{
	StructType::StructType(ContainedTypes types) : types(std::move(types)) {}

	std::weak_ordering StructType::operator<=>(const StructType& other) const
	{
		return std::lexicographical_compare_three_way(std::begin(types),
		                                              std::end(types),
		                                              std::begin(other.types),
		                                              std::end(other.types),
		                                              [](const auto& lhs, const auto& rhs) { return lhs <=> rhs; });
	}

	StructType::operator std::string() const
	{
		std::stringstream ss;
		ss << '(';
		for (size_t index = 0; index < types.size(); ++index) {
			ss << std::string(types[index]);
			if (index + 1 != types.size()) {
				ss << ", ";
			}
		}
		ss << ')';
		return ss.str();
	}

	std::ostream& operator<<(std::ostream& os, const StructType& type) { return os << std::string(type); }
}  // namespace soul::types
