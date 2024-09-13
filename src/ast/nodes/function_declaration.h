#pragma once

#include "ast/ast.h"

#include <vector>

namespace soul
{
	/**
	 * @brief Represents a 'Function Declaration' statement
	 * in the Abstract Syntax Tree (AST).
	 */
	class FunctionDeclarationNode final : public ASTNodeAcceptor<FunctionDeclarationNode>
	{
		public:
			using dependency_t = ASTNode::dependency_t;
			using dependencies_t = ASTNode::dependencies_t;
			using identifier_t = ASTNode::identifier_t;

		private:
			identifier_t   _identifier;
			identifier_t   _return_type_identifier;
			dependencies_t _parameters;
			dependencies_t _statements;

		public:
			explicit FunctionDeclarationNode(identifier_t&& identifier,
			                                 identifier_t&& return_type_identifier,
			                                 dependencies_t&& parameters,
			                                 dependencies_t&& statements);
			~FunctionDeclarationNode() override = default;

			/**
			 * @brief Constructs new Function Declaration statement node.
			 * @param identifier
			 * @param returntype_identifier
			 * @param parameters
			 * @param statements
			 * @return New 'Function Declaration' statement node.
			 */
			static dependency_t create(identifier_t&& identifier,
				                       identifier_t&& return_type_identifier,
				                       dependencies_t&& parameters,
				                       dependencies_t&& statements);
	};
}  // namespace soul
