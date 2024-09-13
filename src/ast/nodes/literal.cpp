#include "ast/nodes/literal.h"

namespace soul
{
	LiteralNode::LiteralNode(const value_t& value)
		: _value(value) {}

	LiteralNode::dependency_t LiteralNode::create(const value_t& value)
	{
		return std::make_unique<LiteralNode>(value);
	}
}  // namespace soul
