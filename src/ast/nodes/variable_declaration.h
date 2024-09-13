#pragma once

#include "ast/ast.h"

namespace soul
{
	/**
	 * @brief Represents a 'Variable Declaration' statement
	 * in the Abstract Syntax Tree (AST).
	 */
	class VariableDeclarationNode final : public ASTNodeAcceptor<VariableDeclarationNode>
	{
		public:
			using dependency_t = ASTNode::dependency_t;
			using identifier_t = ASTNode::identifier_t;

		private:
			identifier_t _identifier;
			identifier_t _type_identifier;
			dependency_t _expression;
			bool         _is_mutable;

		public:
			explicit VariableDeclarationNode(identifier_t&& identifier,
			                                 identifier_t&& type_identifier,
			                                 dependency_t&& expression,
			                                 bool is_mutable);
			~VariableDeclarationNode() override = default;

			/**
			 * @brief Constructs new Variable Declaration statement node.
			 * @param identifier Name of the Variable to be identified by.
			 * @param type_identifier String Type of the variable. Used for Type checking.
			 * @param expression Expression this variable evaluates to.
			 * @param is_mutable Can the value of the expression be reassigned, i.e. is const?
			 * @return New 'Variable Declaration' statement node.
			 */
			static dependency_t create(identifier_t&& identifier, identifier_t&& type_identifier, dependency_t&& expression, bool is_mutable);
	};
}
