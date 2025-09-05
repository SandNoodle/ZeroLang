#pragma once

#include "ast/ast.h"
#include "ast/nodes/nodes_fwd.h"
#include "ast/visitors/visitor.h"

#include <functional>

namespace soul::ast::visitors
{
#define SOUL_VISIT_NODE_OVERRIDE_DEFINE(node_name) \
	virtual void visit(node_name&) override;       \
	virtual void visit(const node_name&) override;

	/**
	 * @brief @TODO
	 */
	class DefaultTraverseVisitor : public IVisitor
	{
		public:
		virtual ~DefaultTraverseVisitor() = default;

		virtual void accept(const ASTNode::Reference node);

		SOUL_VISIT_NODE_OVERRIDE_DEFINE(nodes::BinaryNode)
		SOUL_VISIT_NODE_OVERRIDE_DEFINE(nodes::BlockNode)
		SOUL_VISIT_NODE_OVERRIDE_DEFINE(nodes::CastNode)
		SOUL_VISIT_NODE_OVERRIDE_DEFINE(nodes::ErrorNode)
		SOUL_VISIT_NODE_OVERRIDE_DEFINE(nodes::ForLoopNode)
		SOUL_VISIT_NODE_OVERRIDE_DEFINE(nodes::ForeachLoopNode)
		SOUL_VISIT_NODE_OVERRIDE_DEFINE(nodes::FunctionDeclarationNode)
		SOUL_VISIT_NODE_OVERRIDE_DEFINE(nodes::IfNode)
		SOUL_VISIT_NODE_OVERRIDE_DEFINE(nodes::LiteralNode)
		SOUL_VISIT_NODE_OVERRIDE_DEFINE(nodes::ModuleNode)
		SOUL_VISIT_NODE_OVERRIDE_DEFINE(nodes::StructDeclarationNode)
		SOUL_VISIT_NODE_OVERRIDE_DEFINE(nodes::UnaryNode)
		SOUL_VISIT_NODE_OVERRIDE_DEFINE(nodes::VariableDeclarationNode)
	};
}  // namespace soul::ast::visitors
