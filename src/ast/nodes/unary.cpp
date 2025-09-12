#include "ast/nodes/unary.h"

namespace soul::ast::nodes
{
	UnaryNode::UnaryNode(Dependency expr, Operator op) : expr(std::move(expr)), op(op) {}

	UnaryNode::Dependency UnaryNode::create(Dependency expr, Operator op)
	{
		return std::make_unique<UnaryNode>(std::move(expr), op);
	}
}  // namespace soul::ast::nodes
