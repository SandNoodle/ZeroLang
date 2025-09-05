#include "ast/nodes/for_loop.h"

namespace soul::ast::nodes
{
	ForLoopNode::ForLoopNode(Dependency initialization,
	                         Dependency condition,
	                         Dependency update,
	                         ScopeBlock statements) noexcept
		: initialization(std::move(initialization)),
		  condition(std::move(condition)),
		  update(std::move(update)),
		  statements(std::move(statements))
	{
	}

	ForLoopNode::Dependency ForLoopNode::create(Dependency initialization,
	                                            Dependency condition,
	                                            Dependency update,
	                                            ScopeBlock statements)
	{
		return std::make_unique<ForLoopNode>(
			std::move(initialization), std::move(condition), std::move(update), std::move(statements));
	}

	ForLoopNode::Dependency ForLoopNode::clone() const
	{
		return create(initialization ? initialization->clone() : nullptr,
		              condition ? condition->clone() : nullptr,
		              update ? update->clone() : nullptr,
		              statements->clone_block());
	}
}  // namespace soul::ast::nodes
