#pragma once

#include "ast/ast.h"

#include <vector>

namespace soul
{
	/**
	 * @brief Represents a 'Struct Declaration' statement
	 * in the Abstract Syntax Tree (AST).
	 */
	class StructDeclarationNode final : public ASTNodeAcceptor<StructDeclarationNode>
	{
		public:
		using dependency_t   = ASTNode::dependency_t;
		using dependencies_t = std::vector<dependency_t>;
		using identifier_t   = ASTNode::identifier_t;

		private:
		identifier_t   _identifier;
		dependencies_t _parameters;

		public:
		explicit StructDeclarationNode(identifier_t&& identifier, dependencies_t&& parameters);
		~StructDeclarationNode() override = default;

		/**
		 * @brief Constructs new Struct Declaration statement node.
		 * @param identifier Name of the Struct to be identified by.
		 * @param statements Variable declarations making this struct.
		 * @return New 'Struct Declaration' statement node.
		 */
		static dependency_t create(identifier_t&& identifier, dependencies_t&& statements);
	};
}  // namespace soul
