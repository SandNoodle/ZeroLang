#include "common/types/struct_type.h"

#include "common/types/type.h"

#include <algorithm>

namespace soul::types
{
	StructType::StructType(ContainedTypes types) : types(std::move(types)) {}

	std::weak_ordering StructType::operator<=>(const StructType& other) const noexcept
	{
		return std::lexicographical_compare_three_way(std::begin(types),
		                                              std::end(types),
		                                              std::begin(other.types),
		                                              std::end(other.types),
		                                              [](const auto& lhs, const auto& rhs) { return lhs <=> rhs; });
	}

}  // namespace soul::types
