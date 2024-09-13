#include "ast/nodes/struct_declaration.h"

namespace soul
{
	StructDeclarationNode::StructDeclarationNode(identifier_t&& identifier, dependencies_t&& parameters)
		: _identifier(std::move(identifier)), _parameters(std::move(parameters))
	{
	}

	StructDeclarationNode::dependency_t StructDeclarationNode::create(identifier_t&&   identifier,
	                                                                  dependencies_t&& parameters)
	{
		return std::make_unique<StructDeclarationNode>(std::move(identifier), std::move(parameters));
	}
}  // namespace soul
