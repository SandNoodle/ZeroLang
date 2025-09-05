#pragma once

#include "ast/ast.h"

namespace soul::ast::nodes
{
	/**
	 * @brief Represents any invalid state, which prevents further processing in the Abstract Syntax Tree (AST).
	 */
	class ErrorNode final : public VisitorAcceptor<ErrorNode>
	{
		public:
		using Message = std::string;

		public:
		Message message;

		public:
		explicit ErrorNode(Message message);
		~ErrorNode() override = default;

		/**
		 * @brief Constructs new Error node.
		 * @param message Error message associated with this node.
		 */
		static Dependency create(Message message);

		[[nodiscard]] Dependency clone() const override;
	};
}  // namespace soul::ast::nodes
