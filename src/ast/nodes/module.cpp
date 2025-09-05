#include "ast/nodes/module.h"

#include <ranges>

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

	ModuleNode::Dependency ModuleNode::clone() const
	{
		auto cloned_statements{ statements
			                    | std::views::transform([](const auto& statement) { return statement->clone(); }) };
		return create(name, ASTNode::Dependencies{ cloned_statements.begin(), cloned_statements.end() });
	}
}  // namespace soul::ast::nodes
