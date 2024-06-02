#pragma once

#include "ast/ast.h"
#include "ast/ast_operator.h"

namespace soul
{
	/**
	 * @brief Represents a 'Binary' expression in the Abstract Syntax Tree (AST).
	 * Contains two operands bound by an operator.
	 */
	class binary_t final : public ast_node_acceptor_t<binary_t>
	{
		public:
			using dependency_t = ast_node_t::dependency_t;

		private:
			ast_node_operator_t _operator;
			dependency_t _lhs;
			dependency_t _rhs;

		public:
			explicit binary_t(dependency_t lhs, dependency_t rhs, ast_node_operator_t op);
			~binary_t() override = default;

			/**
			 * @brief Constructs new Binary expression node.
			 * @param lhs Left-hand side expression.
			 * @param rhs Right-hand side expression.
			 * @param op Operator binding the two sides together.
			 * @return New 'Binary' expression node.
			 */
			static dependency_t create(dependency_t lhs, dependency_t rhs, ast_node_operator_t op);
	};
}  // namespace soul
