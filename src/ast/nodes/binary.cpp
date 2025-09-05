#include "ast/nodes/binary.h"

namespace soul::ast::nodes
{
	BinaryNode::BinaryNode(Dependency lhs, Dependency rhs, ASTNodeOperator op)
		: lhs(std::move(lhs)), rhs(std::move(rhs)), op(op)
	{
	}

	BinaryNode::Dependency BinaryNode::create(Dependency lhs, Dependency rhs, ASTNodeOperator op)
	{
		return std::make_unique<BinaryNode>(std::move(lhs), std::move(rhs), op);
	}

	BinaryNode::Dependency BinaryNode::clone() const
	{
		return create(lhs ? lhs->clone() : nullptr, rhs ? rhs->clone() : nullptr, op);
	}
}  // namespace soul::ast::nodes
