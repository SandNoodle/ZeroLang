#pragma once

#include "ast/nodes/nodes_fwd.h"

#include <utility>

namespace soul::ast::visitors
{
#define SOUL_VISIT_NODE_DEFINE(node_name)                                         \
	virtual constexpr void visit(node_name& node) { visit(std::as_const(node)); } \
	virtual constexpr void visit(const node_name&) { /* Does nothing. */ }

	/**
	 * @brief Base class for visiting nodes in the Abstract Syntax Tree.
	 */
	class IVisitor
	{
		public:
		virtual ~IVisitor() = default;

		SOUL_VISIT_NODE_DEFINE(nodes::BinaryNode)
		SOUL_VISIT_NODE_DEFINE(nodes::CastNode)
		SOUL_VISIT_NODE_DEFINE(nodes::ForLoopNode)
		SOUL_VISIT_NODE_DEFINE(nodes::ForeachLoopNode)
		SOUL_VISIT_NODE_DEFINE(nodes::FunctionDeclarationNode)
		SOUL_VISIT_NODE_DEFINE(nodes::IfNode)
		SOUL_VISIT_NODE_DEFINE(nodes::LiteralNode)
		SOUL_VISIT_NODE_DEFINE(nodes::ModuleNode)
		SOUL_VISIT_NODE_DEFINE(nodes::StructDeclarationNode)
		SOUL_VISIT_NODE_DEFINE(nodes::UnaryNode)
		SOUL_VISIT_NODE_DEFINE(nodes::VariableDeclarationNode)
	};
#undef SOUL_VISIT_NODE_DEFINE
}  // namespace soul::ast::visitors
