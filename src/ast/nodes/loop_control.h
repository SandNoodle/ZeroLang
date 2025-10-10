#pragma once

#include "ast/ast.h"

namespace soul::ast::nodes
{
	/**
	 * @brief Represents a `Continue/Break` statement (in loops) in the Abstract Syntax Tree (AST).
	 */
	class LoopControlNode final : public VisitorAcceptor<LoopControlNode>
	{
		public:
		enum class Type : bool;

		public:
		Type control_type;

		public:
		explicit LoopControlNode(Type control_type) noexcept;
		~LoopControlNode() override = default;

		/**
		 * @brief Construct new LoopControl node.
		 * @param control_type Type of the control statement, either `break` or `continue.
		 * @return New 'LoopControlNode' node.
		 */
		static Dependency create(Type control_type);
	};

	enum class LoopControlNode::Type : bool
	{
		Break,
		Continue,
	};
}  // namespace soul::ast::nodes
