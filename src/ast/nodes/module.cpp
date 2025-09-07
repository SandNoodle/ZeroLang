#include "ast/nodes/module.h"

namespace soul::ast::nodes
{
	ModuleNode::ModuleNode(Identifier module_name, Dependencies statements) noexcept
		: name(std::move(module_name)), statements(std::move(statements))
	{
	}

	ASTNode::Dependency ModuleNode::create(Identifier module_name, Dependencies statements)
	{
		return std::make_unique<ModuleNode>(std::move(module_name), std::move(statements));
	}
}  // namespace soul::ast::nodes
