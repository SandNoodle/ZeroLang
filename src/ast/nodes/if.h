#pragma once

#include "ast/ast.h"

namespace soul::ast::nodes
{

	/**
	 * @brief
	 */
	class IfNode final : public VisitorAcceptor<IfNode>
	{
		public:
		using Dependency   = ASTNode::Dependency;
		using Dependencies = ASTNode::Dependencies;

		public:
		Dependency   condition;
		Dependencies if_statements;
		Dependencies else_statements;

		public:
		explicit IfNode(Dependency condition, Dependencies if_statements, Dependencies else_statements) noexcept;
		~IfNode() override = default;

		/**
		 * @brief
		 * @param condition
		 * @param if_statements
		 * @param else_statements [Optional]
		 */
		static Dependency create(Dependency condition, Dependencies if_statements, Dependencies else_statements);
	};

}  // namespace soul::ast::nodes
