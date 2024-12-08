#include "ast/nodes/foreach_loop.h"

namespace soul
{
	ForeachLoopNode::ForeachLoopNode(Dependency variable, Dependency in_expression, Dependencies statements) noexcept
		: variable(std::move(variable)), in_expression(std::move(in_expression)), statements(std::move(statements))
	{
	}

	ForeachLoopNode::Dependency ForeachLoopNode::create(Dependency   variable,
	                                                    Dependency   in_expression,
	                                                    Dependencies statements)
	{
		return std::make_unique<ForeachLoopNode>(std::move(variable), std::move(in_expression), std::move(statements));
	}

}  // namespace soul
