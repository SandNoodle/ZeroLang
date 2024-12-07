#include "ast/visitors/default_traverse.h"

#include "ast/nodes/assign.h"
#include "ast/nodes/binary.h"
#include "ast/nodes/function_declaration.h"
#include "ast/nodes/literal.h"
#include "ast/nodes/struct_declaration.h"
#include "ast/nodes/unary.h"
#include "ast/nodes/variable_declaration.h"

#include <utility>

namespace soul
{

	void DefaultTraverseVisitor::accept(const ASTNode::Reference node)
	{
		if (!node) {
			return;
		}
		node->accept(*this);
	}

	void DefaultTraverseVisitor::visit(AssignNode& node) { visit(std::as_const(node)); }
	void DefaultTraverseVisitor::visit(BinaryNode& node) { visit(std::as_const(node)); }
	void DefaultTraverseVisitor::visit(BlockNode& node) { visit(std::as_const(node)); }
	void DefaultTraverseVisitor::visit(FunctionDeclarationNode& node) { visit(std::as_const(node)); }
	void DefaultTraverseVisitor::visit(LiteralNode& node) { visit(std::as_const(node)); }
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

	void DefaultTraverseVisitor::visit(const BlockNode& node)
	{
		// TODO.
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

	void DefaultTraverseVisitor::visit(const LiteralNode& node) { /* Can't traverse further */ }

	void DefaultTraverseVisitor::visit(const StructDeclarationNode& node)
	{
		for (auto& param : node.parameters) {
			accept(param.get());
		}
	}

	void DefaultTraverseVisitor::visit(const UnaryNode& node) { accept(node.expr.get()); }

	void DefaultTraverseVisitor::visit(const VariableDeclarationNode& node) { accept(node.expr.get()); }
}  // namespace soul
