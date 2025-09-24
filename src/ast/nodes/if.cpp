#include "ast/nodes/if.h"

namespace soul::ast::nodes
{

	IfNode::IfNode(Dependency condition, ScopeBlock then_statements, ScopeBlock else_statements) noexcept
		: condition(std::move(condition)),
		  then_statements(std::move(then_statements)),
		  else_statements(std::move(else_statements))
	{
	}

	IfNode::Dependency IfNode::create(Dependency condition, ScopeBlock then_statements, ScopeBlock else_statements)
	{
		return std::make_unique<IfNode>(std::move(condition), std::move(then_statements), std::move(else_statements));
	}
}  // namespace soul::ast::nodes
