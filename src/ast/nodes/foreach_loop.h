#pragma once

#include "ast/ast.h"
#include "ast/nodes/block.h"

namespace soul::ast::nodes
{
	/**
	 * @brief Represents any conditional 'ForeachLoop' expression statement
	 * in the Abstract Syntax Tree (AST).
	 */
	class ForeachLoopNode final : public VisitorAcceptor<ForeachLoopNode>
	{
		public:
		Dependency variable;
		Dependency in_expression;
		ScopeBlock statements;

		public:
		explicit ForeachLoopNode(Dependency variable, Dependency in_expression, ScopeBlock statements) noexcept;
		~ForeachLoopNode() override = default;

		/**
		 * @brief Constructs new ForeachLoop expression statement node.
		 * @param variable Expression representing value of each iteration.
		 * @param in_expression Expression to iterate upon.
		 * @param statements List of statements to execute each loop.
		 * @return New 'ForeachLoop' expression statement node.
		 */
		static Dependency create(Dependency variable, Dependency in_expression, ScopeBlock statements);
	};
}  // namespace soul::ast::nodes
