#include "ast/nodes/assign.h"

namespace soul
{
	AssignNode::AssignNode(Dependency lhs, Dependency rhs) : lhs(std::move(lhs)), rhs(std::move(rhs)) {}

	AssignNode::Dependency AssignNode::create(Dependency lhs, Dependency rhs)
	{
		return std::make_unique<AssignNode>(std::move(lhs), std::move(rhs));
	}
}  // namespace soul
