#include "ast/nodes/binary.h"

namespace soul::ast::nodes
{
	BinaryNode::BinaryNode(Dependency lhs, Dependency rhs, Operator op)
		: lhs(std::move(lhs)), rhs(std::move(rhs)), op(op)
	{
	}

	BinaryNode::Dependency BinaryNode::create(Dependency lhs, Dependency rhs, Operator op)
	{
		return std::make_unique<BinaryNode>(std::move(lhs), std::move(rhs), op);
	}
}  // namespace soul::ast::nodes
