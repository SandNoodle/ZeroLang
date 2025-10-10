#include "ast/nodes/return.h"

namespace soul::ast::nodes
{
	ReturnNode::ReturnNode(Dependency expression) : expression(std::move(expression)) {}

	ReturnNode::Dependency ReturnNode::create(Dependency expression)
	{
		return std::make_unique<ReturnNode>(std::move(expression));
	}
}  // namespace soul::ast::nodes
