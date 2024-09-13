#pragma once

#include "ast/ast.h"
#include "ast/ast_operator.h"

namespace soul
{
	/**
	 * @brief Represents a 'Binary' expression in the Abstract Syntax Tree (AST).
	 * Contains two operands bound by an operator.
	 */
	class BinaryNode final : public ASTNodeAcceptor<BinaryNode>
	{
		public:
			using dependency_t = ASTNode::dependency_t;

		private:
			ASTNodeOperator _operator;
			dependency_t _lhs;
			dependency_t _rhs;

		public:
			explicit BinaryNode(dependency_t lhs, dependency_t rhs, ASTNodeOperator op);
			~BinaryNode() override = default;

			/**
			 * @brief Constructs new Binary expression node.
			 * @param lhs Left-hand side expression.
			 * @param rhs Right-hand side expression.
			 * @param op Operator binding the two sides together.
			 * @return New 'Binary' expression node.
			 */
			static dependency_t create(dependency_t lhs, dependency_t rhs, ASTNodeOperator op);
	};
}  // namespace soul
