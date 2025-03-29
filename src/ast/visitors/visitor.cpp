#include "ast/visitors/visitor.h"

#include <utility>

namespace soul::ast::visitors
{
	using namespace ast::nodes;

	void IVisitor::visit(AssignNode& node) { visit(std::as_const(node)); }
	void IVisitor::visit(BinaryNode& node) { visit(std::as_const(node)); }
	void IVisitor::visit(CastNode& node) { visit(std::as_const(node)); }
	void IVisitor::visit(ForLoopNode& node) { visit(std::as_const(node)); }
	void IVisitor::visit(ForeachLoopNode& node) { visit(std::as_const(node)); }
	void IVisitor::visit(FunctionDeclarationNode& node) { visit(std::as_const(node)); }
	void IVisitor::visit(IfNode& node) { visit(std::as_const(node)); }
	void IVisitor::visit(LiteralNode& node) { visit(std::as_const(node)); }
	void IVisitor::visit(ModuleNode& node) { visit(std::as_const(node)); }
	void IVisitor::visit(StructDeclarationNode& node) { visit(std::as_const(node)); }
	void IVisitor::visit(UnaryNode& node) { visit(std::as_const(node)); }
	void IVisitor::visit(VariableDeclarationNode& node) { visit(std::as_const(node)); }

	void IVisitor::visit(const AssignNode& node) { /* Does nothing. */ }
	void IVisitor::visit(const BinaryNode& node) { /* Does nothing. */ }
	void IVisitor::visit(const CastNode& node) { /* Does nothing. */ }
	void IVisitor::visit(const ForLoopNode&) { /* Does nothing. */ }
	void IVisitor::visit(const ForeachLoopNode&) { /* Does nothing. */ }
	void IVisitor::visit(const FunctionDeclarationNode&) { /* Does nothing. */ }
	void IVisitor::visit(const IfNode& node) { /* Does nothing. */ }
	void IVisitor::visit(const LiteralNode&) { /* Does nothing. */ }
	void IVisitor::visit(const ModuleNode&) { /* Does nothing. */ }
	void IVisitor::visit(const StructDeclarationNode&) { /* Does nothing. */ }
	void IVisitor::visit(const UnaryNode&) { /* Does nothing. */ }
	void IVisitor::visit(const VariableDeclarationNode&) { /* Does nothing. */ }

}  // namespace soul::ast::visitors
