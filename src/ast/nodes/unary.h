#pragma once

#include "ast/ast.h"

namespace soul::ast::nodes
{
	/**
	 * @brief Represents an 'Unary' expression in the Abstract Syntax Tree (AST).
	 * Contains an operand bound by an operator.
	 */
	class UnaryNode final : public VisitorAcceptor<UnaryNode>
	{
		public:
		Operator   op;
		Dependency expression;

		public:
		explicit UnaryNode(Dependency expr, Operator op);
		~UnaryNode() override = default;

		/**
		 * @brief Constructs new Unary expression node.
		 * @param expr Expression to be bound by an operator.
		 * @param op Operator binding the expression.
		 * @return New 'Unary' expression node.
		 */
		static Dependency create(Dependency expr, Operator op);
	};
}  // namespace soul::ast::nodes
