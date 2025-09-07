#pragma once

#include "ast/ast.h"

#include <vector>

namespace soul::ast::nodes
{
	/**
	 * @brief Represents a 'Struct Declaration' statement
	 * in the Abstract Syntax Tree (AST).
	 */
	class StructDeclarationNode final : public VisitorAcceptor<StructDeclarationNode>
	{
		public:
		Identifier   name;
		Dependencies parameters;

		public:
		explicit StructDeclarationNode(Identifier name, Dependencies parameters);
		~StructDeclarationNode() override = default;

		/**
		 * @brief Constructs new Struct Declaration statement node.
		 * @param name Name of the Struct to be identified by.
		 * @param statements Variable declarations making this struct.
		 * @return New 'Struct Declaration' statement node.
		 */
		static Dependency create(Identifier name, Dependencies statements);
	};
}  // namespace soul::ast::nodes
