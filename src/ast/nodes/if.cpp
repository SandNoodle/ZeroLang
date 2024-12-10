#include "ast/nodes/if.h"

namespace soul
{

	IfNode::IfNode(Dependency condition, Dependencies if_statements, Dependencies else_statements) noexcept
		: condition(std::move(condition)),
		  if_statements(std::move(if_statements)),
		  else_statements(std::move(else_statements))
	{
	}

	IfNode::Dependency IfNode::create(Dependency condition, Dependencies if_statements, Dependencies else_statements)
	{
		return std::make_unique<IfNode>(std::move(condition), std::move(if_statements), std::move(else_statements));
	}

}  // namespace soul
