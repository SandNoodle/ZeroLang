#pragma once

#include "ast/ast.h"
#include "ast/nodes/block.h"

namespace soul::ast::nodes
{
	/**
	 * @brief Represents a 'Function Declaration' statement
	 * in the Abstract Syntax Tree (AST).
	 */
	class FunctionDeclarationNode final : public VisitorAcceptor<FunctionDeclarationNode>
	{
		public:
		Identifier   name;
		Identifier   type_identifier;
		Dependencies parameters;
		ScopeBlock   statements;

		public:
		explicit FunctionDeclarationNode(Identifier   identifier,
		                                 Identifier   return_type_identifier,
		                                 Dependencies parameters,
		                                 ScopeBlock   statements);
		~FunctionDeclarationNode() override = default;

		/**
		 * @brief Constructs new Function Declaration statement node.
		 * @param name Name of the function.
		 * @param return_type Type which this function returns.
		 * @param parameters [Optional] Parameters this function takes.
		 * @param statements [Optional] Statements this function executes.
		 * @return New 'Function Declaration' statement node.
		 */
		static Dependency create(Identifier   name,
		                         Identifier   return_type,
		                         Dependencies parameters,
		                         ScopeBlock   statements);
	};
}  // namespace soul::ast::nodes
