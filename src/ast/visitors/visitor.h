#pragma once

#include "ast/nodes/nodes_fwd.h"

#include <utility>

namespace soul::ast::visitors
{
	/**
	 * @brief Base class for visiting nodes in the Abstract Syntax Tree.
	 */
	class IVisitor
	{
		public:
		virtual ~IVisitor() = default;

		/**
		 * @brief Returns true if a visitor modifies or affects the AST in any shape or form, i.e. does not perform
		 * `read-only` operations.
		 */
		[[nodiscard]] virtual constexpr bool affects() const noexcept { return false; }

		virtual constexpr void visit(const nodes::BinaryNode&) {}
		virtual constexpr void visit(const nodes::BlockNode&) {}
		virtual constexpr void visit(const nodes::CastNode&) {}
		virtual constexpr void visit(const nodes::ErrorNode&) {}
		virtual constexpr void visit(const nodes::ForLoopNode&) {}
		virtual constexpr void visit(const nodes::ForeachLoopNode&) {}
		virtual constexpr void visit(const nodes::FunctionCallNode&) {}
		virtual constexpr void visit(const nodes::FunctionDeclarationNode&) {}
		virtual constexpr void visit(const nodes::IfNode&) {}
		virtual constexpr void visit(const nodes::LiteralNode&) {}
		virtual constexpr void visit(const nodes::ModuleNode&) {}
		virtual constexpr void visit(const nodes::StructDeclarationNode&) {}
		virtual constexpr void visit(const nodes::UnaryNode&) {}
		virtual constexpr void visit(const nodes::VariableDeclarationNode&) {}
		virtual constexpr void visit(const nodes::WhileNode&) {}

		virtual constexpr void visit(nodes::BinaryNode& node) { visit(std::as_const(node)); }
		virtual constexpr void visit(nodes::BlockNode& node) { visit(std::as_const(node)); }
		virtual constexpr void visit(nodes::CastNode& node) { visit(std::as_const(node)); }
		virtual constexpr void visit(nodes::ErrorNode& node) { visit(std::as_const(node)); }
		virtual constexpr void visit(nodes::ForLoopNode& node) { visit(std::as_const(node)); }
		virtual constexpr void visit(nodes::ForeachLoopNode& node) { visit(std::as_const(node)); }
		virtual constexpr void visit(nodes::FunctionCallNode& node) { visit(std::as_const(node)); }
		virtual constexpr void visit(nodes::FunctionDeclarationNode& node) { visit(std::as_const(node)); }
		virtual constexpr void visit(nodes::IfNode& node) { visit(std::as_const(node)); }
		virtual constexpr void visit(nodes::LiteralNode& node) { visit(std::as_const(node)); }
		virtual constexpr void visit(nodes::ModuleNode& node) { visit(std::as_const(node)); }
		virtual constexpr void visit(nodes::StructDeclarationNode& node) { visit(std::as_const(node)); }
		virtual constexpr void visit(nodes::UnaryNode& node) { visit(std::as_const(node)); }
		virtual constexpr void visit(nodes::VariableDeclarationNode& node) { visit(std::as_const(node)); }
		virtual constexpr void visit(nodes::WhileNode& node) { visit(std::as_const(node)); }
	};
}  // namespace soul::ast::visitors
