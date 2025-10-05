#include "ast/nodes/while.h"

namespace soul::ast::nodes
{
	WhileNode::WhileNode(ASTNode::Dependency condition, ASTNode::ScopeBlock statements) noexcept
		: condition(std::move(condition)), statements(std::move(statements))
	{
	}

	ASTNode::Dependency WhileNode::create(ASTNode::Dependency condition, ASTNode::ScopeBlock statements)
	{
		return std::make_unique<WhileNode>(std::move(condition), std::move(statements));
	}

}  // namespace soul::ast::nodes
