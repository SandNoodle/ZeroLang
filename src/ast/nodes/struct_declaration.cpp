#include "ast/nodes/struct_declaration.h"

#include <ranges>

namespace soul::ast::nodes
{
	StructDeclarationNode::StructDeclarationNode(Identifier name, Dependencies parameters)
		: name(std::move(name)), parameters(std::move(parameters))
	{
	}

	StructDeclarationNode::Dependency StructDeclarationNode::create(Identifier name, Dependencies parameters)
	{
		return std::make_unique<StructDeclarationNode>(std::move(name), std::move(parameters));
	}

	StructDeclarationNode::Dependency StructDeclarationNode::clone() const
	{
		auto cloned_parameters{ parameters
			                    | std::views::transform([](const auto& parameter) { return parameter->clone(); }) };
		return create(name, ASTNode::Dependencies{ cloned_parameters.begin(), cloned_parameters.end() });
	}
}  // namespace soul::ast::nodes
