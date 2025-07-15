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
		Dependency expression      = nullptr;
		Identifier type_identifier = {};

		public:
		explicit CastNode(Dependency expression, Identifier type_identifier);
		~CastNode() override = default;

		/**
		 * @brief Constructs new Cast expression node.
		 * @param expr Expression to cast.
		 * @param type_identifier Type to cast to.
		 * @return New 'Cast' expression node.
		 */
		static Dependency create(Dependency expr, Identifier type_identifier);
	};
}  // namespace soul::ast::nodes
