#pragma once

#include "ast/ast.h"

namespace soul::ast::nodes
{
	/**
	 * @brief Represents a 'Binary' expression in the Abstract Syntax Tree (AST).
	 * Contains two operands bound by an operator.
	 */
	class BinaryNode final : public VisitorAcceptor<BinaryNode>
	{
		public:
		Operator   op  = Operator::Unknown;
		Dependency lhs = nullptr;
		Dependency rhs = nullptr;

		public:
		explicit BinaryNode(Dependency lhs, Dependency rhs, Operator op);
		~BinaryNode() override = default;

		/**
		 * @brief Constructs new Binary expression node.
		 * @param lhs Left-hand side expression.
		 * @param rhs Right-hand side expression.
		 * @param op Operator binding the two sides together.
		 * @return New 'Binary' expression node.
		 */
		static Dependency create(Dependency lhs, Dependency rhs, Operator op);
	};
}  // namespace soul::ast::nodes
