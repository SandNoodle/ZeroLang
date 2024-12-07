#pragma once

#include "ast/ast.h"

namespace soul
{

	/**
	 * @brief Represents any conditional 'Loop' expression statement
	 * in the Abstract Syntax Tree (AST), such as `for` or `while` loops.
	 */
	class LoopNode final : public VisitorAcceptor<LoopNode>
	{
		public:
		using Dependency = ASTNode::Dependency;

		public:
		Dependency   initialization;
		Dependency   condition;
		Dependency   update;
		Dependencies statements;

		public:
		explicit LoopNode(Dependency   initialization,
		                  Dependency   condition,
		                  Dependency   update,
		                  Dependencies statements) noexcept;
		~LoopNode() override = default;

		/**
		 * @brief Constructs new Loop expression statement node.
		 * @param initialization [Optional] Expression executed once before the start of the loop.
		 * @param condition [Optional] Expression verified after each loop iteration.
		 * @param update [Optional] Expression updated after each loop iteration (after condition expression).
		 * @param statements List of statements to execute each loop.
		 * @return New 'Loop' expression statement node.
		 */
		static Dependency create(Dependency   initialization,
		                         Dependency   condition,
		                         Dependency   update,
		                         Dependencies statements);
	};

}  // namespace soul
