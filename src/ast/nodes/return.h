#pragma once

#include "ast/ast.h"

namespace soul::ast::nodes
{
	/**
	 * @brief Represents a `Return` statement in the Abstract Syntax Tree (AST).
	 */
	class ReturnNode final : public VisitorAcceptor<ReturnNode>
	{
		public:
		Dependency expression;

		public:
		explicit ReturnNode(Dependency expression);
		~ReturnNode() override = default;

		/**
		 * @brief Construct new Retrun node.
		 * @param expression [Optional] Expression to be returned.
		 * @return New 'ReturNode  node.
		 */
		static Dependency create(Dependency expression = {});
	};
}  // namespace soul::ast::nodes
