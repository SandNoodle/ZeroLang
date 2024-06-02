#include "ast/nodes/function_declaration.h"

namespace soul
{
	function_declaration_t::function_declaration_t(const identifier_t& identifier,
				                                   const identifier_t& return_type_identifier,
				                                   dependencies_t&& parameters,
				                                   dependencies_t&& statements)
		: _identifier(identifier),
		  _return_type_identifier(return_type_identifier),
		  _parameters(std::move(parameters)),
		  _statements(std::move(statements)) {}

	function_declaration_t::dependency_t function_declaration_t::create(
		const identifier_t& identifier,
		const identifier_t& return_type_identifier,
		dependencies_t&& parameters,
		dependencies_t&& statements)
	{
		return std::make_unique<function_declaration_t>(identifier, return_type_identifier, std::move(parameters), std::move(statements));
	}
}  // namespace soul
