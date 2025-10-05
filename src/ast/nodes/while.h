#pragma once

#include "ast/ast.h"
#include "ast/nodes/block.h"

namespace soul::ast::nodes
{
	class WhileNode final : public VisitorAcceptor<WhileNode>
	{
		public:
		Dependency condition;
		ScopeBlock statements;

		public:
		explicit WhileNode(Dependency condition, ScopeBlock statements) noexcept;
		~WhileNode() override = default;

		/**
		 * @brief Constructs new WhileLoop expression statement node.
		 * @param condition Expression verified after each loop iteration.
		 * @param statements List of statements to execute each loop.
		 * @return New 'WhileLoop' expression statement node.
		 */
		static Dependency create(Dependency condition, ScopeBlock statements);
	};
}  // namespace soul::ast::nodes
