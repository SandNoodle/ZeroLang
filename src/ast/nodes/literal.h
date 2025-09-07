#pragma once

#include "ast/ast.h"
#include "common/value.h"
#include "core/types.h"

namespace soul::ast::nodes
{
	/**
	 * @brief Represents a 'Literal' expression in the Abstract Syntax Tree (AST).
	 */
	class LiteralNode : public VisitorAcceptor<LiteralNode>
	{
		public:
		Value value = {};

		public:
		LiteralNode(Value value);
		~LiteralNode() override = default;
		operator std::string() const noexcept;

		/**
		 * @brief Constructs new Literal node.
		 * @param type
		 * @param value
		 * @return
		 */
		static Dependency create(Value value);
	};
}  // namespace soul::ast::nodes
