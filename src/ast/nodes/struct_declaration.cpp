#include "ast/nodes/struct_declaration.h"

namespace soul
{
	struct_declaration_t::struct_declaration_t(identifier_t&& identifier,
				                               dependencies_t&& parameters)
		: _identifier(std::move(identifier)),
		  _parameters(std::move(parameters)) {}

	struct_declaration_t::dependency_t struct_declaration_t::create(
		identifier_t&& identifier,
		dependencies_t&& parameters)
	{
		return std::make_unique<struct_declaration_t>(std::move(identifier),
			                                          std::move(parameters));
	}
}  // namespace soul
