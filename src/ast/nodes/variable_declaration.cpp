#include "ast/nodes/variable_declaration.h"

namespace soul
{
	variable_declaration_t::variable_declaration_t(identifier_t&& identifier,
				                                   identifier_t&& type_identifier,
				                                   dependency_t&& expression,
				                                   bool is_mutable)
		: _identifier(std::move(identifier)),
		  _type_identifier(std::move(type_identifier)),
		  _expression(std::move(expression)),
		  _is_mutable(is_mutable) {}

	variable_declaration_t::dependency_t variable_declaration_t::create(
		identifier_t&& identifier,
		identifier_t&& type_identifier,
		dependency_t&& expression,
		bool is_mutable)
	{
		return std::make_unique<variable_declaration_t>(std::move(identifier),
			                                            std::move(type_identifier),
			                                            std::move(expression),
			                                            is_mutable);
	}
}  // namespace soul
