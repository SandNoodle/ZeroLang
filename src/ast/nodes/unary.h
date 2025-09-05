#pragma once

#include "ast/ast.h"
#include "ast/ast_operator.h"

namespace soul::ast::nodes
{
	/**
	 * @brief Represents an 'Unary' expression in the Abstract Syntax Tree (AST).
	 * Contains an operand bound by an operator.
	 */
	class UnaryNode final : public VisitorAcceptor<UnaryNode>
	{
		public:
		ASTNodeOperator op;
		Dependency      expr;

		public:
		explicit UnaryNode(Dependency expr, ASTNodeOperator op);
		~UnaryNode() override = default;

		/**
		 * @brief Constructs new Unary expression node.
		 * @param expr Expression to be bound by an operator.
		 * @param op Operator binding the expression.
		 * @return New 'Unary' expression node.
		 */
		static Dependency create(Dependency expr, ASTNodeOperator op);

		[[nodiscard]] Dependency clone() const override;
	};
}  // namespace soul::ast::nodes
