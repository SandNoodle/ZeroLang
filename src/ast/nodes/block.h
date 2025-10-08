#pragma once

#include "ast/ast.h"

namespace soul::ast::nodes
{
	/**
	 * @brief Represents an array of statements (in the same scope) in the Abstract Syntax Tree (AST).
	 */
	class BlockNode : public VisitorAcceptor<BlockNode>
	{
		public:
		Dependencies statements = {};

		public:
		explicit BlockNode(Dependencies dependencies);
		~BlockNode() override = default;

		/**
		 * @brief Constructs new Block expression node.
		 * @param statements
		 * @return New 'Block' expression node.
		 */
		static Dependency create(Dependencies statements);
	};
}  // namespace soul::ast::nodes
