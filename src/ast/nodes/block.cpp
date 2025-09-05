#include "ast/nodes/block.h"

#include <ranges>

namespace soul::ast::nodes
{
	BlockNode::BlockNode(Dependencies dependencies) : statements(std::move(dependencies)) {}

	BlockNode::ScopeBlock BlockNode::create(BlockNode::Dependencies statements)
	{
		return std::make_unique<BlockNode>(std::move(statements));
	}

	BlockNode::Dependency BlockNode::clone() const { return clone_block(); }

	[[nodiscard]] BlockNode::ScopeBlock BlockNode::clone_block() const
	{
		auto cloned_statements{ statements
			                    | std::views::transform([](const auto& statement) { return statement->clone(); }) };
		return BlockNode::create(ASTNode::Dependencies{ cloned_statements.begin(), cloned_statements.end() });
	}
}  // namespace soul::ast::nodes
