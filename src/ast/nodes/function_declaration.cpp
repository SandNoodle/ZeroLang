#include "ast/nodes/function_declaration.h"

namespace soul
{
	FunctionDeclarationNode::FunctionDeclarationNode(identifier_t&& identifier,
	                                                 identifier_t&& return_type_identifier,
	                                                 dependencies_t&& parameters,
	                                                 dependencies_t&& statements)
		: _identifier(std::move(identifier)),
		  _return_type_identifier(std::move(return_type_identifier)),
		  _parameters(std::move(parameters)),
		  _statements(std::move(statements)) {}

	FunctionDeclarationNode::dependency_t FunctionDeclarationNode::create(
		identifier_t&& identifier,
		identifier_t&& return_type_identifier,
		dependencies_t&& parameters,
		dependencies_t&& statements)
	{
		return std::make_unique<FunctionDeclarationNode>(std::move(identifier),
		                                                 std::move(return_type_identifier),
		                                                 std::move(parameters),
		                                                 std::move(statements));
	}
}  // namespace soul
