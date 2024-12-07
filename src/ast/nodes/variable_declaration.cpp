#include "ast/nodes/variable_declaration.h"

namespace soul
{
	VariableDeclarationNode::VariableDeclarationNode(Identifier name, Identifier type, Dependency expr, bool is_mutable)
		: name(std::move(name)), type_identifier(std::move(type)), expr(std::move(expr)), is_mutable(is_mutable)
	{
	}

	VariableDeclarationNode::Dependency VariableDeclarationNode::create(Identifier name,
	                                                                    Identifier type,
	                                                                    Dependency expr,
	                                                                    bool       is_mutable)
	{
		return std::make_unique<VariableDeclarationNode>(std::move(name), std::move(type), std::move(expr), is_mutable);
	}
}  // namespace soul
