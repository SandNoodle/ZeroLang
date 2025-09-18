#include "ast/nodes/function_declaration.h"

namespace soul::ast::nodes
{
	FunctionDeclarationNode::FunctionDeclarationNode(Identifier   identifier,
	                                                 Identifier   return_type_identifier,
	                                                 Dependencies parameters,
	                                                 ScopeBlock   statements)
		: name(std::move(identifier)),
		  type_identifier(std::move(return_type_identifier)),
		  parameters(std::move(parameters)),
		  statements(std::move(statements))
	{
	}

	FunctionDeclarationNode::Dependency FunctionDeclarationNode::create(Identifier   name,
	                                                                    Identifier   return_type,
	                                                                    Dependencies parameters,
	                                                                    ScopeBlock   statements)
	{
		return std::make_unique<FunctionDeclarationNode>(
			std::move(name), std::move(return_type), std::move(parameters), std::move(statements));
	}
}  // namespace soul::ast::nodes
