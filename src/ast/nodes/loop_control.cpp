#include "ast/nodes/loop_control.h"

namespace soul::ast::nodes
{
	LoopControlNode::LoopControlNode(Type control_type) noexcept : control_type(control_type) {}

	LoopControlNode::Dependency LoopControlNode::create(Type control_type)
	{
		return std::make_unique<LoopControlNode>(control_type);
	}
}  // namespace soul::ast::nodes
