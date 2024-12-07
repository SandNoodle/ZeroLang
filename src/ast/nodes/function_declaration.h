#pragma once

#include "ast/ast.h"

namespace soul
{
	/**
	 * @brief Represents a 'Function Declaration' statement
	 * in the Abstract Syntax Tree (AST).
	 */
	class FunctionDeclarationNode final : public VisitorAcceptor<FunctionDeclarationNode>
	{
		public:
		using Dependency   = ASTNode::Dependency;
		using Dependencies = ASTNode::Dependencies;
		using Identifier   = ASTNode::Identifier;

		public:
		Identifier   name;
		Identifier   return_type;
		Dependencies parameters;
		Dependencies statements;

		public:
		explicit FunctionDeclarationNode(Identifier   identifier,
		                                 Identifier   return_type_identifier,
		                                 Dependencies parameters,
		                                 Dependencies statements);
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
		                         Dependencies statements);
	};
}  // namespace soul
