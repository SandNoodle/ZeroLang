#include "ast/visitors/default_traverse.h"

#include "ast/nodes/binary.h"
#include "ast/nodes/block.h"
#include "ast/nodes/cast.h"
#include "ast/nodes/for_loop.h"
#include "ast/nodes/foreach_loop.h"
#include "ast/nodes/function_call.h"
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

	void DefaultTraverseVisitor::accept(ASTNode::Reference node)
	{
		if (!node) {
			return;
		}
		node->accept(*this);
	}

	void DefaultTraverseVisitor::visit(const BinaryNode& node)
	{
		accept(node.lhs.get());
		accept(node.rhs.get());
	}

	void DefaultTraverseVisitor::visit(const BlockNode& node)
	{
		for (const auto& statement : node.statements) {
			accept(statement.get());
		}
	}

	void DefaultTraverseVisitor::visit(const CastNode& node) { accept(node.expression.get()); }

	void DefaultTraverseVisitor::visit([[maybe_unused]] const ErrorNode& node) { /* Can't traverse further. */ }

	void DefaultTraverseVisitor::visit(const ForLoopNode& node)
	{
		accept(node.initialization.get());
		accept(node.condition.get());
		accept(node.update.get());
		accept(node.statements.get());
	}

	void DefaultTraverseVisitor::visit(const ForeachLoopNode& node)
	{
		accept(node.variable.get());
		accept(node.in_expression.get());
		accept(node.statements.get());
	}

	void DefaultTraverseVisitor::visit(const FunctionCallNode& node)
	{
		for (auto& param : node.parameters) {
			accept(param.get());
		}
	}

	void DefaultTraverseVisitor::visit(const FunctionDeclarationNode& node)
	{
		for (auto& param : node.parameters) {
			accept(param.get());
		}
		accept(node.statements.get());
	}

	void DefaultTraverseVisitor::visit(const IfNode& node)
	{
		accept(node.condition.get());
		accept(node.then_statements.get());
		accept(node.else_statements.get());
	}

	void DefaultTraverseVisitor::visit([[maybe_unused]] const LiteralNode& node) { /* Can't traverse further. */ }

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

	void DefaultTraverseVisitor::visit(const UnaryNode& node) { accept(node.expression.get()); }

	void DefaultTraverseVisitor::visit(const VariableDeclarationNode& node) { accept(node.expression.get()); }

	void DefaultTraverseVisitor::visit(BinaryNode& node) { visit(std::as_const(node)); }
	void DefaultTraverseVisitor::visit(BlockNode& node) { visit(std::as_const(node)); }
	void DefaultTraverseVisitor::visit(CastNode& node) { visit(std::as_const(node)); }
	void DefaultTraverseVisitor::visit(ErrorNode& node) { visit(std::as_const(node)); }
	void DefaultTraverseVisitor::visit(ForLoopNode& node) { visit(std::as_const(node)); }
	void DefaultTraverseVisitor::visit(ForeachLoopNode& node) { visit(std::as_const(node)); }
	void DefaultTraverseVisitor::visit(FunctionCallNode& node) { visit(std::as_const(node)); }
	void DefaultTraverseVisitor::visit(FunctionDeclarationNode& node) { visit(std::as_const(node)); }
	void DefaultTraverseVisitor::visit(IfNode& node) { visit(std::as_const(node)); }
	void DefaultTraverseVisitor::visit(LiteralNode& node) { visit(std::as_const(node)); }
	void DefaultTraverseVisitor::visit(ModuleNode& node) { visit(std::as_const(node)); }
	void DefaultTraverseVisitor::visit(StructDeclarationNode& node) { visit(std::as_const(node)); }
	void DefaultTraverseVisitor::visit(UnaryNode& node) { visit(std::as_const(node)); }
	void DefaultTraverseVisitor::visit(VariableDeclarationNode& node) { visit(std::as_const(node)); }
}  // namespace soul::ast::visitors
