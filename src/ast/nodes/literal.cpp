#include "ast/nodes/literal.h"

namespace soul::ast::nodes
{
	LiteralNode::LiteralNode(Value value, Type literal_type) : value(std::move(value)), literal_type(literal_type) {}

	LiteralNode::Dependency LiteralNode::create(Value value, Type literal_type)
	{
		return std::make_unique<LiteralNode>(std::move(value), literal_type);
	}

	LiteralNode::operator std::string() const noexcept { return std::string(value); }
}  // namespace soul::ast::nodes
