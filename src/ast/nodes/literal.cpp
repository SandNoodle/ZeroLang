#include "ast/nodes/literal.h"

namespace soul::ast::nodes
{
	LiteralNode::LiteralNode(Value value) : value(std::move(value)) {}

	LiteralNode::Dependency LiteralNode::create(Value value) { return std::make_unique<LiteralNode>(std::move(value)); }

	LiteralNode::operator std::string() const noexcept { return std::string(value); }
}  // namespace soul::ast::nodes
