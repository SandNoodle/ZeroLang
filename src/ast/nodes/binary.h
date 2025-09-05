#pragma once

#include "ast/ast.h"
#include "ast/ast_operator.h"

namespace soul::ast::nodes
{
	/**
	 * @brief Represents a 'Binary' expression in the Abstract Syntax Tree (AST).
	 * Contains two operands bound by an operator.
	 */
	class BinaryNode final : public VisitorAcceptor<BinaryNode>
	{
		public:
		ASTNodeOperator op  = ASTNodeOperator::Unknown;
		Dependency      lhs = nullptr;
		Dependency      rhs = nullptr;

		public:
		explicit BinaryNode(Dependency lhs, Dependency rhs, ASTNodeOperator op);
		~BinaryNode() override = default;

		/**
		 * @brief Constructs new Binary expression node.
		 * @param lhs Left-hand side expression.
		 * @param rhs Right-hand side expression.
		 * @param op Operator binding the two sides together.
		 * @return New 'Binary' expression node.
		 */
		static Dependency create(Dependency lhs, Dependency rhs, ASTNodeOperator op);

		[[nodiscard]] Dependency clone() const override;
	};
}  // namespace soul::ast::nodes
