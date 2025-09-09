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

		protected:
		virtual void visit(const nodes::BinaryNode&) override;
		virtual void visit(const nodes::BlockNode&) override;
		virtual void visit(const nodes::CastNode&) override;
		virtual void visit(const nodes::ErrorNode&) override;
		virtual void visit(const nodes::ForLoopNode&) override;
		virtual void visit(const nodes::ForeachLoopNode&) override;
		virtual void visit(const nodes::FunctionDeclarationNode&) override;
		virtual void visit(const nodes::IfNode&) override;
		virtual void visit(const nodes::LiteralNode&) override;
		virtual void visit(const nodes::ModuleNode&) override;
		virtual void visit(const nodes::StructDeclarationNode&) override;
		virtual void visit(const nodes::UnaryNode&) override;
		virtual void visit(const nodes::VariableDeclarationNode&) override;

		virtual void visit(nodes::BinaryNode&) override;
		virtual void visit(nodes::BlockNode&) override;
		virtual void visit(nodes::CastNode&) override;
		virtual void visit(nodes::ErrorNode&) override;
		virtual void visit(nodes::ForLoopNode&) override;
		virtual void visit(nodes::ForeachLoopNode&) override;
		virtual void visit(nodes::FunctionDeclarationNode&) override;
		virtual void visit(nodes::IfNode&) override;
		virtual void visit(nodes::LiteralNode&) override;
		virtual void visit(nodes::ModuleNode&) override;
		virtual void visit(nodes::StructDeclarationNode&) override;
		virtual void visit(nodes::UnaryNode&) override;
		virtual void visit(nodes::VariableDeclarationNode&) override;
	};
}  // namespace soul::ast::visitors
