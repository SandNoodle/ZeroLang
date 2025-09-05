#include "ast/nodes/literal.h"

#include <sstream>
#include <utility>

namespace soul::ast::nodes
{
	LiteralNode::LiteralNode(Value value) : value(std::move(value)) {}

	LiteralNode::Dependency LiteralNode::create(Value value) { return std::make_unique<LiteralNode>(std::move(value)); }

	LiteralNode::operator std::string() const noexcept { return std::string(value); }

	LiteralNode::Dependency LiteralNode::clone() const { return create(value); }
}  // namespace soul::ast::nodes
