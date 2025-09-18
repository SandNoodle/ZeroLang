#pragma once

#include "ast/ast.h"
#include "ast/nodes/nodes_fwd.h"
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
		virtual void visit(const nodes::BinaryNode&) override;
		virtual void visit(const nodes::BlockNode&) override;
		virtual void visit(const nodes::CastNode&) override;
		virtual void visit(const nodes::ErrorNode&) override;
		virtual void visit(const nodes::ForLoopNode&) override;
		virtual void visit(const nodes::ForeachLoopNode&) override;
		virtual void visit(const nodes::FunctionCallNode&) override;
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
		virtual void visit(nodes::FunctionCallNode&) override;
		virtual void visit(nodes::FunctionDeclarationNode&) override;
		virtual void visit(nodes::IfNode&) override;
		virtual void visit(nodes::LiteralNode&) override;
		virtual void visit(nodes::ModuleNode&) override;
		virtual void visit(nodes::StructDeclarationNode&) override;
		virtual void visit(nodes::UnaryNode&) override;
		virtual void visit(nodes::VariableDeclarationNode&) override;

		/**
		 * @brief Verifies if a Node is of a given (node) type.
		 * @tparam T Type that satisfies the
		 * @return \p true if it is, \b false otherwise.
		 */
		template <nodes::NodeKind T>
		constexpr bool is(ASTNode::Reference node) const noexcept
		{
			return dynamic_cast<T*>(node) != nullptr;
		}
	};
}  // namespace soul::ast::visitors
