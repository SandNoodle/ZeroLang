#include "ast/nodes/struct_declaration.h"

namespace soul
{
	StructDeclarationNode::StructDeclarationNode(Identifier name, Dependencies parameters)
		: name(std::move(name)), parameters(std::move(parameters))
	{
	}

	StructDeclarationNode::Dependency StructDeclarationNode::create(Identifier name, Dependencies parameters)
	{
		return std::make_unique<StructDeclarationNode>(std::move(name), std::move(parameters));
	}
}  // namespace soul
