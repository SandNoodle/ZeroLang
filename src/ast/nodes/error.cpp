#include "ast/nodes/error.h"

namespace soul::ast::nodes
{
	ErrorNode::ErrorNode(ErrorNode::Message message) : message(std::move(message)) {}

	ASTNode::Dependency ErrorNode::create(ErrorNode::Message message)
	{
		return std::make_unique<ErrorNode>(std::move(message));
	}
}  // namespace soul::ast::nodes
