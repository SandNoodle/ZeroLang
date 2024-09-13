#include "ast/nodes/binary.h"

namespace soul
{
	BinaryNode::BinaryNode(dependency_t lhs, dependency_t rhs, ASTNodeOperator op)
		: _lhs(std::move(lhs)), _rhs(std::move(rhs)), _operator(op)
	{
	}

	BinaryNode::dependency_t BinaryNode::create(dependency_t lhs, dependency_t rhs, ASTNodeOperator op)
	{
		return std::make_unique<BinaryNode>(std::move(lhs), std::move(rhs), op);
	}
}  // namespace soul
