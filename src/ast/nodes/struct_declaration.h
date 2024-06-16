#pragma once

#include "ast/ast.h"

#include <vector>

namespace soul
{
	/**
	 * @brief Represents a 'Struct Declaration' statement
	 * in the Abstract Syntax Tree (AST).
	 */
	class struct_declaration_t final : public ast_node_acceptor_t<struct_declaration_t>
	{
		public:
			using dependency_t = ast_node_t::dependency_t;
			using dependencies_t = std::vector<dependency_t>;
			using identifier_t = ast_node_t::identifier_t;

		private:
			identifier_t   _identifier;
			dependencies_t _parameters;

		public:
			explicit struct_declaration_t(identifier_t&& identifier,
				                          dependencies_t&& parameters);
			~struct_declaration_t() override = default;

			/**
			 * @brief Constructs new Struct Declaration statement node.
			 * @param identifier Name of the Struct to be identified by.
			 * @param statements Variable declarations making this struct.
			 * @return New 'Struct Declaration' statement node.
			 */
			static dependency_t create(identifier_t&& identifier,
				                       dependencies_t&& statements);
	};
}  // namespace soul
