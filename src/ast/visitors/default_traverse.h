#pragma once

#include "ast/ast.h"
#include "ast/ast_fwd.h"
#include "ast/visitors/visitor.h"

#include <functional>

namespace soul::ast::visitors
{
	/**
	 * @brief DefaultTraverseVisitor traverses every node of the Abstract Syntax Tree.
	 * It's useful for getting into a specific node.
	 */
	class DefaultTraverseVisitor : public IVisitor
	{
		public:
		virtual ~DefaultTraverseVisitor() = default;

		virtual void accept(ASTNode::Reference node);

		protected:
		virtual void visit(const BinaryNode&) override;
		virtual void visit(const BlockNode&) override;
		virtual void visit(const CastNode&) override;
		virtual void visit(const ErrorNode&) override;
		virtual void visit(const ForLoopNode&) override;
		virtual void visit(const ForeachLoopNode&) override;
		virtual void visit(const FunctionCallNode&) override;
		virtual void visit(const FunctionDeclarationNode&) override;
		virtual void visit(const IfNode&) override;
		virtual void visit(const LiteralNode&) override;
		virtual void visit(const LoopControlNode&) override;
		virtual void visit(const ModuleNode&) override;
		virtual void visit(const ReturnNode&) override;
		virtual void visit(const StructDeclarationNode&) override;
		virtual void visit(const UnaryNode&) override;
		virtual void visit(const VariableDeclarationNode&) override;
		virtual void visit(const WhileNode&) override;

		virtual void visit(BinaryNode&) override;
		virtual void visit(BlockNode&) override;
		virtual void visit(CastNode&) override;
		virtual void visit(ErrorNode&) override;
		virtual void visit(ForLoopNode&) override;
		virtual void visit(ForeachLoopNode&) override;
		virtual void visit(FunctionCallNode&) override;
		virtual void visit(FunctionDeclarationNode&) override;
		virtual void visit(IfNode&) override;
		virtual void visit(LiteralNode&) override;
		virtual void visit(LoopControlNode&) override;
		virtual void visit(ModuleNode&) override;
		virtual void visit(ReturnNode&) override;
		virtual void visit(StructDeclarationNode&) override;
		virtual void visit(UnaryNode&) override;
		virtual void visit(VariableDeclarationNode&) override;
		virtual void visit(WhileNode&) override;
	};
}  // namespace soul::ast::visitors
