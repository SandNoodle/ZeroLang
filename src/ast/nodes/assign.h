#pragma once

#include "ast/ast.h"

namespace soul
{
	/**
	 * @brief Represents a 'Assign' expression in the Abstract Syntax Tree (AST).
	 * Contains an expression assigned to another expression.
	 */
	class assign_t : public ast_node_acceptor_t<assign_t>
	{
		public:
			using dependency_t = ast_node_t::dependency_t;

		private:
			dependency_t _lhs;
			dependency_t _rhs;

		public:
			explicit assign_t(dependency_t lhs, dependency_t rhs);
			~assign_t() override = default;

			/**
			 * @brief Constructs new Assign expression node.
			 * @param lhs Expression to assign to.
			 * @param rhs Expression to assign from.
			 * @return New 'Assign' expression node.
			 */
			static dependency_t create(dependency_t lhs, dependency_t rhs);
	};
}  // namespace soul
