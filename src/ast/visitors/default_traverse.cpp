#include "ast/visitors/default_traverse.h"

#include "ast/nodes/assign.h"
#include "ast/nodes/binary.h"
#include "ast/nodes/cast.h"
#include "ast/nodes/for_loop.h"
#include "ast/nodes/foreach_loop.h"
#include "ast/nodes/function_declaration.h"
#include "ast/nodes/if.h"
#include "ast/nodes/literal.h"
#include "ast/nodes/module.h"
#include "ast/nodes/struct_declaration.h"
#include "ast/nodes/unary.h"
#include "ast/nodes/variable_declaration.h"

#include <utility>

namespace soul::ast::visitors
{
	using namespace soul::ast::nodes;

	void DefaultTraverseVisitor::accept(const ASTNode::Reference node)
	{
		if (!node) {
			return;
		}
		node->accept(*this);
	}

	void DefaultTraverseVisitor::visit(AssignNode& node) { visit(std::as_const(node)); }
	void DefaultTraverseVisitor::visit(BinaryNode& node) { visit(std::as_const(node)); }
	void DefaultTraverseVisitor::visit(CastNode& node) { visit(std::as_const(node)); }
	void DefaultTraverseVisitor::visit(ForLoopNode& node) { visit(std::as_const(node)); }
	void DefaultTraverseVisitor::visit(ForeachLoopNode& node) { visit(std::as_const(node)); }
	void DefaultTraverseVisitor::visit(FunctionDeclarationNode& node) { visit(std::as_const(node)); }
	void DefaultTraverseVisitor::visit(IfNode& node) { visit(std::as_const(node)); }
	void DefaultTraverseVisitor::visit(LiteralNode& node) { visit(std::as_const(node)); }
	void DefaultTraverseVisitor::visit(ModuleNode& node) { visit(std::as_const(node)); }
	void DefaultTraverseVisitor::visit(StructDeclarationNode& node) { visit(std::as_const(node)); }
	void DefaultTraverseVisitor::visit(UnaryNode& node) { visit(std::as_const(node)); }
	void DefaultTraverseVisitor::visit(VariableDeclarationNode& node) { visit(std::as_const(node)); }

	void DefaultTraverseVisitor::visit(const AssignNode& node)
	{
		accept(node.lhs.get());
		accept(node.rhs.get());
	}

	void DefaultTraverseVisitor::visit(const BinaryNode& node)
	{
		accept(node.lhs.get());
		accept(node.rhs.get());
	}

	void DefaultTraverseVisitor::visit(const CastNode& node) { accept(node.expression.get()); }

	void DefaultTraverseVisitor::visit(const ForLoopNode& node)
	{
		accept(node.initialization.get());
		accept(node.condition.get());
		accept(node.update.get());
		for (const auto& statements : node.statements) {
			accept(statements.get());
		}
	}

	void DefaultTraverseVisitor::visit(const ForeachLoopNode& node)
	{
		accept(node.variable.get());
		accept(node.in_expression.get());
		for (const auto& statements : node.statements) {
			accept(statements.get());
		}
	}

	void DefaultTraverseVisitor::visit(const FunctionDeclarationNode& node)
	{
		for (auto& param : node.parameters) {
			accept(param.get());
		}

		for (auto& statement : node.statements) {
			accept(statement.get());
		}
	}

	void DefaultTraverseVisitor::visit(const IfNode& node)
	{
		accept(node.condition.get());
		for (const auto& statement : node.if_statements) {
			accept(statement.get());
		}
		for (const auto& statement : node.else_statements) {
			accept(statement.get());
		}
	}

	void DefaultTraverseVisitor::visit(const LiteralNode& node) { /* Can't traverse further. */ }

	void DefaultTraverseVisitor::visit(const ModuleNode& node)
	{
		for (const auto& statement : node.statements) {
			accept(statement.get());
		}
	}

	void DefaultTraverseVisitor::visit(const StructDeclarationNode& node)
	{
		for (auto& param : node.parameters) {
			accept(param.get());
		}
	}

	void DefaultTraverseVisitor::visit(const UnaryNode& node) { accept(node.expr.get()); }

	void DefaultTraverseVisitor::visit(const VariableDeclarationNode& node) { accept(node.expr.get()); }
}  // namespace soul::ast::visitors
