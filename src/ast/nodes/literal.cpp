#include "ast/nodes/literal.h"

#include <utility>

namespace soul
{
	LiteralNode::LiteralNode(ValueType value) : value(std::move(value)) {}

	LiteralNode::Dependency LiteralNode::create(ValueType value)
	{
		return std::make_unique<LiteralNode>(std::move(value));
	}
}  // namespace soul
