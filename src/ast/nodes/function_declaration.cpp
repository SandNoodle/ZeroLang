#include "ast/nodes/function_declaration.h"

#include <ranges>

namespace soul::ast::nodes
{
	FunctionDeclarationNode::FunctionDeclarationNode(Identifier   identifier,
	                                                 Identifier   return_type_identifier,
	                                                 Dependencies parameters,
	                                                 ScopeBlock   statements)
		: name(std::move(identifier)),
		  return_type(std::move(return_type_identifier)),
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

	FunctionDeclarationNode::Dependency FunctionDeclarationNode::clone() const
	{
		auto cloned_parameters{ parameters
			                    | std::views::transform([](const auto& parameter) { return parameter->clone(); }) };
		return create(name,
		              return_type,
		              ASTNode::Dependencies{ cloned_parameters.begin(), cloned_parameters.end() },
		              statements->clone_block());
	}
}  // namespace soul::ast::nodes
