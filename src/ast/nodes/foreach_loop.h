#pragma once

#include "ast/ast.h"

namespace soul::ast::nodes
{

	/**
	 * @brief Represents any conditional 'ForeachLoop' expression statement
	 * in the Abstract Syntax Tree (AST).
	 */
	class ForeachLoopNode final : public VisitorAcceptor<ForeachLoopNode>
	{
		public:
		using Dependency = ASTNode::Dependency;

		public:
		Dependency   variable;
		Dependency   in_expression;
		Dependencies statements;

		public:
		explicit ForeachLoopNode(Dependency variable, Dependency in_expression, Dependencies statements) noexcept;
		~ForeachLoopNode() override = default;

		/**
		 * @brief Constructs new ForeachLoop expression statement node.
		 * @param variable Expression representing value of each iteration.
		 * @param in_expression Expression to iterate upon.
		 * @param statements List of statements to execute each loop.
		 * @return New 'ForeachLoop' expression statement node.
		 */
		static Dependency create(Dependency variable, Dependency in_expression, Dependencies statements);
	};
}  // namespace soul::ast::nodes
