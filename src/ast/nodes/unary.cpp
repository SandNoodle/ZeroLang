#include "ast/nodes/unary.h"

namespace soul::ast::nodes
{
	UnaryNode::UnaryNode(Dependency expr, ASTNodeOperator op) : expr(std::move(expr)), op(op) {}

	UnaryNode::Dependency UnaryNode::create(Dependency expr, ASTNodeOperator op)
	{
		return std::make_unique<UnaryNode>(std::move(expr), op);
	}

	UnaryNode::Dependency UnaryNode::clone() const { return create(expr ? expr->clone() : nullptr, op); }
}  // namespace soul::ast::nodes
