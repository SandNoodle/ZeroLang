#include "ast/ast.h"

namespace soul::ast::nodes
{
	/**
	 * @brief Represents a 'root' node containing all the statements of a given script
	 * associated under common name in the Abstract Syntax Tree (AST)
	 */
	class ModuleNode : public VisitorAcceptor<ModuleNode>
	{
		public:
		using Identifier   = ASTNode::Identifier;
		using Dependency   = ASTNode::Dependency;
		using Dependencies = ASTNode::Dependencies;

		public:
		Identifier   name;
		Dependencies statements;

		public:
		explicit ModuleNode(Identifier module_name, Dependencies statements) noexcept;
		~ModuleNode() override = default;

		/**
		 * @brief Construct new Module node.
		 * @param module_name Name of the module.
		 * @param statements All the statements making up the module.
		 * @return new 'Module' node.
		 */
		static Dependency create(Identifier module_name, Dependencies statements);
	};

}  // namespace soul::ast::nodes
