#include "ast/nodes/unary.h"

namespace soul
{
	UnaryNode::UnaryNode(dependency_t expr, ASTNodeOperator op) : _expr(std::move(expr)), _operator(op) {}

	UnaryNode::dependency_t UnaryNode::create(dependency_t expr, ASTNodeOperator op)
	{
		return std::make_unique<UnaryNode>(std::move(expr), op);
	}
}  // namespace soul
