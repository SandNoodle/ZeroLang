#include "ast/nodes/cast.h"

namespace soul::ast::nodes
{
	CastNode::CastNode(Identifier type_identifier, Dependency expression)
		: type_identifier(std::move(type_identifier)), expression(std::move(expression))
	{
	}

	CastNode::Dependency CastNode::create(Identifier type_identifier, Dependency expression)
	{
		return std::make_unique<CastNode>(std::move(type_identifier), std::move(expression));
	}
}  // namespace soul::ast::nodes
