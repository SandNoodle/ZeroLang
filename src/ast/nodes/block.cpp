#include "ast/nodes/block.h"

namespace soul::ast::nodes
{
	BlockNode::BlockNode(Dependencies dependencies) : statements(std::move(dependencies)) {}

	BlockNode::ScopeBlock BlockNode::create(BlockNode::Dependencies statements)
	{
		return std::make_unique<BlockNode>(std::move(statements));
	}

}  // namespace soul::ast::nodes
