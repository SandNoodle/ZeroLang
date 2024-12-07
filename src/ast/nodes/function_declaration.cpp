#include "ast/nodes/function_declaration.h"

namespace soul
{
	FunctionDeclarationNode::FunctionDeclarationNode(Identifier   identifier,
	                                                 Identifier   return_type_identifier,
	                                                 Dependencies parameters,
	                                                 Dependencies statements)
		: name(std::move(identifier)),
		  return_type(std::move(return_type_identifier)),
		  parameters(std::move(parameters)),
		  statements(std::move(statements))
	{
	}

	FunctionDeclarationNode::Dependency FunctionDeclarationNode::create(Identifier   name,
	                                                                    Identifier   return_type,
	                                                                    Dependencies parameters,
	                                                                    Dependencies statements)
	{
		return std::make_unique<FunctionDeclarationNode>(
			std::move(name), std::move(return_type), std::move(parameters), std::move(statements));
	}
}  // namespace soul
