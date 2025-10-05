#pragma once

#include "ast/visitors/default_traverse.h"

#include <cassert>

namespace soul::ast::visitors
{
	/**
	 * @brief CopyVisitor traverses the AST and performs a deep copy on each node, which is then put into a separate
	 * AST. Both trees are equal to each other, but exist as a separate objects in memory. It's most useful when a
	 * visitor can/must modify the input tree.
	 */
	class CopyVisitor : public DefaultTraverseVisitor
	{
		protected:
		ASTNode::Dependency _current_clone{};

		public:
		/** @brief Returns the cloned (sub-) tree. */
		ASTNode::Dependency cloned() noexcept;

		using DefaultTraverseVisitor::accept;

		[[nodiscard]] constexpr bool affects() const noexcept override { return true; }

		protected:
		using DefaultTraverseVisitor::visit;
		void visit(const nodes::BinaryNode&) override;
		void visit(const nodes::BlockNode&) override;
		void visit(const nodes::CastNode&) override;
		void visit(const nodes::ErrorNode&) override;
		void visit(const nodes::ForLoopNode&) override;
		void visit(const nodes::ForeachLoopNode&) override;
		void visit(const nodes::FunctionCallNode&) override;
		void visit(const nodes::FunctionDeclarationNode&) override;
		void visit(const nodes::IfNode&) override;
		void visit(const nodes::LiteralNode&) override;
		void visit(const nodes::ModuleNode&) override;
		void visit(const nodes::StructDeclarationNode&) override;
		void visit(const nodes::UnaryNode&) override;
		void visit(const nodes::VariableDeclarationNode&) override;
		void visit(const nodes::WhileNode&) override;

		ASTNode::Dependency   clone(const ASTNode::Reference node);
		ASTNode::Dependency   clone(const nodes::BinaryNode&);
		ASTNode::ScopeBlock   clone(const nodes::BlockNode&);
		ASTNode::Dependency   clone(const nodes::CastNode&);
		ASTNode::Dependency   clone(const nodes::ErrorNode&);
		ASTNode::Dependency   clone(const nodes::ForLoopNode&);
		ASTNode::Dependency   clone(const nodes::ForeachLoopNode&);
		ASTNode::Dependency   clone(const nodes::FunctionCallNode&);
		ASTNode::Dependency   clone(const nodes::FunctionDeclarationNode&);
		ASTNode::Dependency   clone(const nodes::IfNode&);
		ASTNode::Dependency   clone(const nodes::LiteralNode&);
		ASTNode::Dependency   clone(const nodes::ModuleNode&);
		ASTNode::Dependency   clone(const nodes::StructDeclarationNode&);
		ASTNode::Dependency   clone(const nodes::UnaryNode&);
		ASTNode::Dependency   clone(const nodes::VariableDeclarationNode&);
		ASTNode::Dependency   clone(const nodes::WhileNode&);
		ASTNode::Dependencies clone(const std::ranges::forward_range auto& elements)
		{
			ASTNode::Dependencies cloned{};
			cloned.reserve(elements.size());
			for (const auto& element : elements) {
				cloned.emplace_back(clone(element.get()));
			}
			return cloned;
		}

		/**
		 * @brief Returns the underlying node.
		 * @important Does not perform any validation - assumes that DefaultTraverseVisitor::is<T> was used first.
		 * @tparam T Type satisfying the NodeKind concept.
		 */
		template <nodes::NodeKind T>
		constexpr T& as()
		{
			assert(dynamic_cast<T*>(_current_clone.get()));
			return dynamic_cast<T&>(*_current_clone);
		}
	};
}  // namespace soul::ast::visitors
