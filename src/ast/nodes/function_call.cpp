#include "ast/nodes/function_call.h"

namespace soul::ast::nodes
{
	FunctionCallNode::FunctionCallNode(Identifier name, Dependencies parameters)
		: name(std::move(name)), parameters(std::move(parameters))
	{
	}

	FunctionCallNode::Dependency FunctionCallNode::create(Identifier name, Dependencies parameters)
	{
		return std::make_unique<FunctionCallNode>(std::move(name), std::move(parameters));
	}
}  // namespace soul::ast::nodes
