#pragma once

#include "ast/ast.h"

namespace soul::ast::nodes
{
	/**
	 * @brief Represents a 'Variable Declaration' statement
	 * in the Abstract Syntax Tree (AST).
	 */
	class VariableDeclarationNode final : public VisitorAcceptor<VariableDeclarationNode>
	{
		public:
		Identifier name            = {};
		Identifier type_identifier = {};
		Dependency expr            = nullptr;
		bool       is_mutable      = false;

		public:
		explicit VariableDeclarationNode(Identifier name, Identifier type, Dependency expr, bool is_mutable);
		~VariableDeclarationNode() override = default;

		/**
		 * @brief Constructs new Variable Declaration statement node.
		 * @param name Name of the Variable to be identified by.
		 * @param type String Type of the variable. Used for Type checking.
		 * @param expr Expression this variable evaluates to.
		 * @param is_mutable Can the value of the expression be reassigned, i.e. is const?
		 * @return New 'Variable Declaration' statement node.
		 */
		static Dependency create(Identifier name, Identifier type, Dependency expr, bool is_mutable);

		[[nodiscard]] Dependency clone() const override;
	};
}  // namespace soul::ast::nodes
