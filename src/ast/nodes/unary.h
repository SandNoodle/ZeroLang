#pragma once

#include "ast/ast.h"
#include "ast/ast_operator.h"

namespace soul
{
	/**
	 * @brief Represents an 'Unary' expression in the Abstract Syntax Tree (AST).
	 * Contains an operand bound by an operator.
	 */
	class UnaryNode final : public ASTNodeAcceptor<UnaryNode>
	{
		public:
			using dependency_t = ASTNode::dependency_t;

		private:
			ASTNodeOperator _operator;
			dependency_t _expr;

		public:
			explicit UnaryNode(dependency_t expr, ASTNodeOperator op);
			~UnaryNode() override = default;

			/**
			 * @brief Constructs new Unary expression node.
			 * @param expr Expression to be bound by an operator.
			 * @param op Operator binding the expression.
			 * @return New 'Unary' expression node.
			 */
			static dependency_t create(dependency_t expr, ASTNodeOperator op);
	};
}  // namespace soul
