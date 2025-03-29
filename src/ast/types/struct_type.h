#pragma once

#include <vector>

namespace soul::ast::types
{
	class Type;

	/**
	 * @brief
	 */
	class StructType
	{
		public:
		using ContainedTypes = std::vector<Type>;

		public:
		ContainedTypes types;

		public:
		StructType(ContainedTypes types);

		bool               operator==(const StructType&) const noexcept = default;
		std::weak_ordering operator<=>(const StructType&) const noexcept;
	};
}  // namespace soul::ast::types
