#include "ast/nodes/assign.h"

namespace soul
{
	assign_t::assign_t(dependency_t lhs, dependency_t rhs)
		: _lhs(std::move(lhs)), _rhs(std::move(rhs)) {}

	assign_t::dependency_t assign_t::create(dependency_t lhs, dependency_t rhs)
	{
		return std::make_unique<assign_t>(std::move(lhs), std::move(rhs));
	}
}  // namespace soul
