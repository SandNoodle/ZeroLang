#include "ast/nodes/unary.h"

namespace soul::ast::nodes
{
	UnaryNode::UnaryNode(Dependency expr, Operator op) : op(op), expression(std::move(expr)) {}

	UnaryNode::Dependency UnaryNode::create(Dependency expr, Operator op)
	{
		return std::make_unique<UnaryNode>(std::move(expr), op);
	}
}  // namespace soul::ast::nodes
