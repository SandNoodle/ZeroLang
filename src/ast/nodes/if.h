#pragma once

#include "ast/ast.h"
#include "ast/nodes/block.h"

namespace soul::ast::nodes
{

	/**
	 * @brief
	 */
	class IfNode final : public VisitorAcceptor<IfNode>
	{
		public:
		Dependency condition;
		ScopeBlock then_statements;
		ScopeBlock else_statements;

		public:
		explicit IfNode(Dependency condition, ScopeBlock then_statements, ScopeBlock else_statements = {}) noexcept;
		~IfNode() override = default;

		/**
		 * @brief
		 * @param condition
		 * @param then_statements
		 * @param else_statements [Optional]
		 */
		static Dependency create(Dependency condition, ScopeBlock then_statements, ScopeBlock else_statements = {});
	};

}  // namespace soul::ast::nodes
