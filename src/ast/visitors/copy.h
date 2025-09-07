#pragma once

#include "ast/visitors/default_traverse.h"

namespace soul::ast::visitors
{
	/**
	 * @brief
	 */
	class CopyVisitor : public DefaultTraverseVisitor
	{
		protected:
		ASTNode::Dependency _cloned_root{};

		public:
		CopyVisitor();

		/**  @brief Returns cloned root node of the (sub)-tree. */
		ASTNode::Dependency cloned() noexcept;

		void accept(const ASTNode::Reference node) override;

		protected:
		using DefaultTraverseVisitor::visit;
		void visit(const nodes::BinaryNode&) override;
		void visit(const nodes::BlockNode&) override;
		void visit(const nodes::CastNode&) override;
		void visit(const nodes::ErrorNode&) override;
		void visit(const nodes::ForLoopNode&) override;
		void visit(const nodes::ForeachLoopNode&) override;
		void visit(const nodes::FunctionDeclarationNode&) override;
		void visit(const nodes::IfNode&) override;
		void visit(const nodes::LiteralNode&) override;
		void visit(const nodes::ModuleNode&) override;
		void visit(const nodes::StructDeclarationNode&) override;
		void visit(const nodes::UnaryNode&) override;
		void visit(const nodes::VariableDeclarationNode&) override;

		ASTNode::Dependency clone(const nodes::BinaryNode&);
		ASTNode::ScopeBlock clone(const nodes::BlockNode&);
		ASTNode::Dependency clone(const nodes::CastNode&);
		ASTNode::Dependency clone(const nodes::ErrorNode&);
		ASTNode::Dependency clone(const nodes::ForLoopNode&);
		ASTNode::Dependency clone(const nodes::ForeachLoopNode&);
		ASTNode::Dependency clone(const nodes::FunctionDeclarationNode&);
		ASTNode::Dependency clone(const nodes::IfNode&);
		ASTNode::Dependency clone(const nodes::LiteralNode&);
		ASTNode::Dependency clone(const nodes::ModuleNode&);
		ASTNode::Dependency clone(const nodes::StructDeclarationNode&);
		ASTNode::Dependency clone(const nodes::UnaryNode&);
		ASTNode::Dependency clone(const nodes::VariableDeclarationNode&);

		ASTNode::Dependency clone(const ASTNode::Reference);
	};
}  // namespace soul::ast::visitors
