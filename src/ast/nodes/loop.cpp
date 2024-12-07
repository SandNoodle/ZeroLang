#include "ast/nodes/loop.h"

namespace soul
{
	LoopNode::LoopNode(Dependency   initialization,
	                   Dependency   condition,
	                   Dependency   update,
	                   Dependencies statements) noexcept
		: initialization(std::move(initialization)),
		  condition(std::move(condition)),
		  update(std::move(update)),
		  statements(std::move(statements))
	{
	}

	LoopNode::Dependency LoopNode::create(Dependency   initialization,
	                                      Dependency   condition,
	                                      Dependency   update,
	                                      Dependencies statements)
	{
		return std::make_unique<LoopNode>(
			std::move(initialization), std::move(condition), std::move(update), std::move(statements));
	}

}  // namespace soul
