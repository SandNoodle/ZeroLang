#pragma once

#include "ast/ast.h"

namespace soul::ast::nodes
{
	/**
	 * @brief Represents a 'Cast' expression in the Abstract Syntax Tree (AST).
	 */
	class CastNode : public VisitorAcceptor<CastNode>
	{
		public:
		using Identifier = ASTNode::Identifier;
		using Dependency = ASTNode::Dependency;

		public:
		Identifier type_identifier = {};
		Dependency expression;

		public:
		explicit CastNode(Identifier type_identifier, Dependency expression);
		~CastNode() override = default;

		/**
		 * @brief Constructs new Cast expression node.
		 * @param type_identifier Type to cast to.
		 * @param rhs Expression to cast from.
		 * @return New 'Cast' expression node.
		 */
		static Dependency create(Identifier type_identifier, Dependency expression);
	};
}  // namespace soul::ast::nodes
