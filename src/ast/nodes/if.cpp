#include "ast/nodes/if.h"

#include <ranges>

namespace soul::ast::nodes
{

	IfNode::IfNode(Dependency condition, ScopeBlock if_statements, ScopeBlock else_statements) noexcept
		: condition(std::move(condition)),
		  if_statements(std::move(if_statements)),
		  else_statements(std::move(else_statements))
	{
	}

	IfNode::Dependency IfNode::create(Dependency condition, ScopeBlock if_statements, ScopeBlock else_statements)
	{
		return std::make_unique<IfNode>(std::move(condition), std::move(if_statements), std::move(else_statements));
	}

	IfNode::Dependency IfNode::clone() const
	{
		return create(
			condition ? condition->clone() : nullptr, if_statements->clone_block(), else_statements->clone_block());
	}
}  // namespace soul::ast::nodes
