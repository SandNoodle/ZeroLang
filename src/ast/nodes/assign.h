#pragma once

#include "ast/ast.h"

namespace soul
{
	/**
	 * @brief Represents a 'Assign' expression in the Abstract Syntax Tree (AST).
	 * Contains an expression assigned to another expression.
	 */
	class AssignNode : public VisitorAcceptor<AssignNode>
	{
		public:
		using Dependency = ASTNode::Dependency;

		public:
		Dependency lhs;
		Dependency rhs;

		public:
		explicit AssignNode(Dependency lhs, Dependency rhs);
		~AssignNode() override = default;

		/**
		 * @brief Constructs new Assign expression node.
		 * @param lhs Expression to assign to.
		 * @param rhs Expression to assign from.
		 * @return New 'Assign' expression node.
		 */
		static Dependency create(Dependency lhs, Dependency rhs);
	};
}  // namespace soul
