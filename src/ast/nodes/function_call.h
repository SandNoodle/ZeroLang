#pragma once

#include "ast/ast.h"

namespace soul::ast::nodes
{
	/**
	 * @brief Represents a Function call statement
	 * in the Abstract Syntax Tree (AST).
	 */
	class FunctionCallNode final : public VisitorAcceptor<FunctionCallNode>
	{
		public:
		Identifier   name;
		Dependencies parameters;

		public:
		explicit FunctionCallNode(Identifier name, Dependencies parameters);
		~FunctionCallNode() override = default;

		/**
		 * @brief Constructs new FunctionCallNode node.
		 * @param name Name of the function.
		 * @param parameters Parameters this function takes.
		 * @return New 'FunctionCallNode' node.
		 */
		static Dependency create(Identifier name, Dependencies parameters);
	};
}  // namespace soul::ast::nodes
