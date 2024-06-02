#pragma once

#include "ast/ast.h"
#include "ast/ast_operator.h"

namespace soul
{
	/**
	 * @brief Represents an 'Unary' expression in the Abstract Syntax Tree (AST).
	 * Contains an operand bound by an operator.
	 */
	class unary_t final : public ast_node_acceptor_t<unary_t>
	{
		public:
			using dependency_t = ast_node_t::dependency_t;

		private:
			ast_node_operator_t _operator;
			dependency_t _expr;

		public:
			explicit unary_t(dependency_t expr, ast_node_operator_t op);
			~unary_t() override = default;

			/**
			 * @brief Constructs new Unary expression node.
			 * @param expr Expression to be bound by an operator.
			 * @param op Operator binding the expression.
			 * @return New 'Unary' expression node.
			 */
			static dependency_t create(dependency_t expr, ast_node_operator_t op);
	};
}  // namespace soul
