#pragma once

#include "ast/ast.h"
#include "ast/nodes/nodes_fwd.h"
#include "ast/visitors/visitor.h"

#include <functional>

namespace soul::ast::visitors
{
	/**
	 * @brief @TODO
	 */
	class DefaultTraverseVisitor : public IVisitor
	{
		public:
		virtual ~DefaultTraverseVisitor() = default;

		virtual void accept(const ASTNode::Reference node);

		virtual void visit(nodes::AssignNode&);
		virtual void visit(nodes::BinaryNode&);
		virtual void visit(nodes::ForLoopNode&);
		virtual void visit(nodes::ForeachLoopNode&);
		virtual void visit(nodes::FunctionDeclarationNode&);
		virtual void visit(nodes::IfNode&);
		virtual void visit(nodes::LiteralNode&);
		virtual void visit(nodes::ModuleNode&);
		virtual void visit(nodes::StructDeclarationNode&);
		virtual void visit(nodes::UnaryNode&);
		virtual void visit(nodes::VariableDeclarationNode&);

		virtual void visit(const nodes::AssignNode&);
		virtual void visit(const nodes::BinaryNode&);
		virtual void visit(const nodes::ForLoopNode&);
		virtual void visit(const nodes::ForeachLoopNode&);
		virtual void visit(const nodes::FunctionDeclarationNode&);
		virtual void visit(const nodes::IfNode&);
		virtual void visit(const nodes::LiteralNode&);
		virtual void visit(const nodes::ModuleNode&);
		virtual void visit(const nodes::StructDeclarationNode&);
		virtual void visit(const nodes::UnaryNode&);
		virtual void visit(const nodes::VariableDeclarationNode&);
	};
}  // namespace soul
