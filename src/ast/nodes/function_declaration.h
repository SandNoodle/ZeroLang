#pragma once

#include "ast/ast.h"

#include <vector>

namespace soul
{
	/**
	 * @brief Represents a 'Function Declaration' statement
	 * in the Abstract Syntax Tree (AST).
	 */
	class function_declaration_t final : public ast_node_acceptor_t<function_declaration_t>
	{
		public:
			using dependency_t = ast_node_t::dependency_t;
			using dependencies_t = ast_node_t::dependencies_t;
			using identifier_t = ast_node_t::identifier_t;

		private:
			identifier_t   _identifier;
			identifier_t   _return_type_identifier;
			dependencies_t _parameters;
			dependencies_t _statements;

		public:
			explicit function_declaration_t(identifier_t&& identifier,
				                            identifier_t&& return_type_identifier,
				                            dependencies_t&& parameters,
				                            dependencies_t&& statements);
			~function_declaration_t() override = default;

			/**
			 * @brief Constructs new Function Declaration statement node.
			 * @param identifier
			 * @param returntype_identifier
			 * @param parameters
			 * @param statements
			 * @return New 'Function Declaration' statement node.
			 */
			static dependency_t create(identifier_t&& identifier,
				                       identifier_t&& return_type_identifier,
				                       dependencies_t&& parameters,
				                       dependencies_t&& statements);
	};
}  // namespace soul
