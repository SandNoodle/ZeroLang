#pragma once

#include "ast/ast.h"

namespace soul
{
	/**
	 * @brief Represents a 'Variable Declaration' statement
	 * in the Abstract Syntax Tree (AST).
	 */
	class variable_declaration_t final : public ast_node_acceptor_t<variable_declaration_t>
	{
		public:
			using dependency_t = ast_node_t::dependency_t;
			using identifier_t = ast_node_t::identifier_t;

		private:
			identifier_t _identifier;
			identifier_t _type_identifier;
			dependency_t _expression;
			bool         _is_mutable = false;

		public:
			explicit variable_declaration_t(const identifier_t& identifier,
				                            const identifier_t& type_identifier,
				                            dependency_t expression,
				                            bool is_mutable = false);
			~variable_declaration_t() override = default;

			/**
			 * @brief Constructs new Variable Declaration statement node.
			 * @param identifier Name of the Variable to be identified by.
			 * @param type_identifier String Type of the variable. Used for Type checking.
			 * @param expression Expression this variable evaluates to.
			 * @param is_mutable Can the value of the expression be reasigned, i.e. is const?
			 * @return New 'Variable Declaration' statement node.
			 */
			static dependency_t create(const identifier_t& identifier, const identifier_t& type_identifier, dependency_t expression, bool is_mutable);
	};
}
