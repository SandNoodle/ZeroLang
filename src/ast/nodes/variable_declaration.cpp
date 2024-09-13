#include "ast/nodes/variable_declaration.h"

namespace soul
{
	VariableDeclarationNode::VariableDeclarationNode(identifier_t&& identifier,
	                                                 identifier_t&& type_identifier,
	                                                 dependency_t&& expression,
	                                                 bool           is_mutable)
		: _identifier(std::move(identifier)),
		  _type_identifier(std::move(type_identifier)),
		  _expression(std::move(expression)),
		  _is_mutable(is_mutable)
	{
	}

	VariableDeclarationNode::dependency_t VariableDeclarationNode::create(identifier_t&& identifier,
	                                                                      identifier_t&& type_identifier,
	                                                                      dependency_t&& expression,
	                                                                      bool           is_mutable)
	{
		return std::make_unique<VariableDeclarationNode>(
			std::move(identifier), std::move(type_identifier), std::move(expression), is_mutable);
	}
}  // namespace soul
