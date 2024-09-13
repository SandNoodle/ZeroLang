#include "ast/nodes/assign.h"

namespace soul
{
	AssignNode::AssignNode(dependency_t lhs, dependency_t rhs)
		: _lhs(std::move(lhs)), _rhs(std::move(rhs)) {}

	AssignNode::dependency_t AssignNode::create(dependency_t lhs, dependency_t rhs)
	{
		return std::make_unique<AssignNode>(std::move(lhs), std::move(rhs));
	}
}  // namespace soul
