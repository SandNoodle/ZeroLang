#pragma once

#include "ast/ast_fwd.h"

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

		virtual constexpr void visit(const BinaryNode&) {}
		virtual constexpr void visit(const BlockNode&) {}
		virtual constexpr void visit(const CastNode&) {}
		virtual constexpr void visit(const ErrorNode&) {}
		virtual constexpr void visit(const ForLoopNode&) {}
		virtual constexpr void visit(const ForeachLoopNode&) {}
		virtual constexpr void visit(const FunctionCallNode&) {}
		virtual constexpr void visit(const FunctionDeclarationNode&) {}
		virtual constexpr void visit(const IfNode&) {}
		virtual constexpr void visit(const LiteralNode&) {}
		virtual constexpr void visit(const LoopControlNode&) {}
		virtual constexpr void visit(const ModuleNode&) {}
		virtual constexpr void visit(const ReturnNode&) {}
		virtual constexpr void visit(const StructDeclarationNode&) {}
		virtual constexpr void visit(const UnaryNode&) {}
		virtual constexpr void visit(const VariableDeclarationNode&) {}
		virtual constexpr void visit(const WhileNode&) {}

		virtual constexpr void visit(BinaryNode& node) { visit(std::as_const(node)); }
		virtual constexpr void visit(BlockNode& node) { visit(std::as_const(node)); }
		virtual constexpr void visit(CastNode& node) { visit(std::as_const(node)); }
		virtual constexpr void visit(ErrorNode& node) { visit(std::as_const(node)); }
		virtual constexpr void visit(ForLoopNode& node) { visit(std::as_const(node)); }
		virtual constexpr void visit(ForeachLoopNode& node) { visit(std::as_const(node)); }
		virtual constexpr void visit(FunctionCallNode& node) { visit(std::as_const(node)); }
		virtual constexpr void visit(FunctionDeclarationNode& node) { visit(std::as_const(node)); }
		virtual constexpr void visit(IfNode& node) { visit(std::as_const(node)); }
		virtual constexpr void visit(LiteralNode& node) { visit(std::as_const(node)); }
		virtual constexpr void visit(LoopControlNode& node) { visit(std::as_const(node)); }
		virtual constexpr void visit(ModuleNode& node) { visit(std::as_const(node)); }
		virtual constexpr void visit(ReturnNode& node) { visit(std::as_const(node)); }
		virtual constexpr void visit(StructDeclarationNode& node) { visit(std::as_const(node)); }
		virtual constexpr void visit(UnaryNode& node) { visit(std::as_const(node)); }
		virtual constexpr void visit(VariableDeclarationNode& node) { visit(std::as_const(node)); }
		virtual constexpr void visit(WhileNode& node) { visit(std::as_const(node)); }
	};
}  // namespace soul::ast::visitors
