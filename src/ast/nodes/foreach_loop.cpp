#include "ast/nodes/foreach_loop.h"

namespace soul::ast::nodes
{
	ForeachLoopNode::ForeachLoopNode(Dependency variable, Dependency in_expression, ScopeBlock statements) noexcept
		: variable(std::move(variable)), in_expression(std::move(in_expression)), statements(std::move(statements))
	{
	}

	ForeachLoopNode::Dependency ForeachLoopNode::create(Dependency   variable,
	                                                    Dependency   in_expression,
	                                                    ScopeBlock statements)
	{
		return std::make_unique<ForeachLoopNode>(std::move(variable), std::move(in_expression), std::move(statements));
	}

}  // namespace soul::ast::nodes
