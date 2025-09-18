#pragma once

#include <string>
#include <vector>

namespace soul::types
{
	class Type;

	/**
	 * @brief Represents a composite data structure that is a collection of (possibly) different data types.
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
		std::weak_ordering operator<=>(const StructType&) const;
		explicit           operator std::string() const;

		friend std::ostream& operator<<(std::ostream& os, const StructType& type);
	};
}  // namespace soul::types
