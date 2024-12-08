#include "ast/nodes/for_loop.h"

namespace soul
{
	ForLoopNode::ForLoopNode(Dependency   initialization,
	                         Dependency   condition,
	                         Dependency   update,
	                         Dependencies statements) noexcept
		: initialization(std::move(initialization)),
		  condition(std::move(condition)),
		  update(std::move(update)),
		  statements(std::move(statements))
	{
	}

	ForLoopNode::Dependency ForLoopNode::create(Dependency   initialization,
	                                            Dependency   condition,
	                                            Dependency   update,
	                                            Dependencies statements)
	{
		return std::make_unique<ForLoopNode>(
			std::move(initialization), std::move(condition), std::move(update), std::move(statements));
	}

}  // namespace soul
