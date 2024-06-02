#include "ast/nodes/variable_declaration.h"

namespace soul
{
	variable_declaration_t::variable_declaration_t(const identifier_t& identifier,
				                                   const identifier_t& type_identifier,
				                                   dependency_t expression,
				                                   bool is_mutable)
		: _identifier(identifier),
		  _type_identifier(type_identifier),
		  _expression(std::move(expression)),
		  _is_mutable(is_mutable) {}

	variable_declaration_t::dependency_t variable_declaration_t::create(const identifier_t& identifier, const identifier_t& type_identifier, dependency_t expression, bool is_mutable)
	{
		return std::make_unique<variable_declaration_t>(identifier, type_identifier, std::move(expression), is_mutable);
	}
}  // namespace soul
