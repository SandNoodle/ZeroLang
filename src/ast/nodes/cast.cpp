#include "ast/nodes/cast.h"

namespace soul::ast::nodes
{
	CastNode::CastNode(Dependency expression, Identifier type_identifier)
		: expression(std::move(expression)), type_identifier(std::move(type_identifier))
	{
	}

	CastNode::Dependency CastNode::create(Dependency expression, Identifier type_identifier)
	{
		return std::make_unique<CastNode>(std::move(expression), std::move(type_identifier));
	}

	CastNode::Dependency CastNode::clone() const
	{
		return create(expression ? expression->clone() : nullptr, type_identifier);
	}
}  // namespace soul::ast::nodes
