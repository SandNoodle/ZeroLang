#include "ast/visitors/copy.h"

#include "ast/nodes/binary.h"
#include "ast/nodes/block.h"
#include "ast/nodes/cast.h"
#include "ast/nodes/error.h"
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
#include "ast/nodes/while.h"

#include <ranges>

namespace soul::ast::visitors
{
	using namespace ast::nodes;

	ASTNode::Dependency CopyVisitor::cloned() noexcept { return std::move(_current_clone); }

	void CopyVisitor::visit(const nodes::BinaryNode& node) { _current_clone = clone(node); }
	void CopyVisitor::visit(const nodes::BlockNode& node) { _current_clone = clone(node); }
	void CopyVisitor::visit(const nodes::CastNode& node) { _current_clone = clone(node); }
	void CopyVisitor::visit(const nodes::ErrorNode& node) { _current_clone = clone(node); }
	void CopyVisitor::visit(const nodes::ForLoopNode& node) { _current_clone = clone(node); }
	void CopyVisitor::visit(const nodes::ForeachLoopNode& node) { _current_clone = clone(node); }
	void CopyVisitor::visit(const nodes::FunctionCallNode& node) { _current_clone = clone(node); }
	void CopyVisitor::visit(const nodes::FunctionDeclarationNode& node) { _current_clone = clone(node); }
	void CopyVisitor::visit(const nodes::IfNode& node) { _current_clone = clone(node); }
	void CopyVisitor::visit(const nodes::LiteralNode& node) { _current_clone = clone(node); }
	void CopyVisitor::visit(const nodes::ModuleNode& node) { _current_clone = clone(node); }
	void CopyVisitor::visit(const nodes::StructDeclarationNode& node) { _current_clone = clone(node); }
	void CopyVisitor::visit(const nodes::UnaryNode& node) { _current_clone = clone(node); }
	void CopyVisitor::visit(const nodes::VariableDeclarationNode& node) { _current_clone = clone(node); }
	void CopyVisitor::visit(const nodes::WhileNode& node) { _current_clone = clone(node); }

	ASTNode::Dependency CopyVisitor::clone(const ASTNode::Reference node)
	{
		if (!node) {
			return nullptr;
		}
		node->accept(*this);
		return std::move(_current_clone);
	}

	ASTNode::Dependency CopyVisitor::clone(const nodes::BinaryNode& node)
	{
		auto lhs{ clone(node.lhs.get()) };
		auto rhs{ clone(node.rhs.get()) };
		return BinaryNode::create(std::move(lhs), std::move(rhs), node.op);
	}

	ASTNode::ScopeBlock CopyVisitor::clone(const nodes::BlockNode& node)
	{
		return BlockNode::create(clone(node.statements));
	}

	ASTNode::Dependency CopyVisitor::clone(const nodes::CastNode& node)
	{
		return CastNode::create(clone(node.expression.get()), node.type_identifier);
	}

	ASTNode::Dependency CopyVisitor::clone(const nodes::ErrorNode& node) { return ErrorNode::create(node.message); }

	ASTNode::Dependency CopyVisitor::clone(const nodes::ForLoopNode& node)
	{
		auto initialization{ clone(node.initialization.get()) };
		auto condition{ clone(node.condition.get()) };
		auto update{ clone(node.update.get()) };
		auto statements{ clone(*static_cast<BlockNode*>(node.statements.get())) };
		return ForLoopNode::create(
			std::move(initialization), std::move(condition), std::move(update), std::move(statements));
	}

	ASTNode::Dependency CopyVisitor::clone(const nodes::ForeachLoopNode& node)
	{
		auto variable{ clone(node.variable.get()) };
		auto in_expression{ clone(node.in_expression.get()) };
		auto statements{ clone(*static_cast<BlockNode*>(node.statements.get())) };
		return ForeachLoopNode::create(std::move(variable), std::move(in_expression), std::move(statements));
	}

	ASTNode::Dependency CopyVisitor::clone(const nodes::FunctionCallNode& node)
	{
		return FunctionCallNode::create(node.name, clone(node.parameters));
	}

	ASTNode::Dependency CopyVisitor::clone(const nodes::FunctionDeclarationNode& node)
	{
		auto cloned_parameters{ clone(node.parameters) };
		auto statements{ clone(*static_cast<BlockNode*>(node.statements.get())) };
		return FunctionDeclarationNode::create(
			node.name, node.type_identifier, std::move(cloned_parameters), std::move(statements));
	}

	ASTNode::Dependency CopyVisitor::clone(const nodes::IfNode& node)
	{
		auto condition{ clone(node.condition.get()) };
		auto then_statements{ clone(*static_cast<BlockNode*>(node.then_statements.get())) };
		auto else_statements{ clone(*static_cast<BlockNode*>(node.else_statements.get())) };
		return IfNode::create(std::move(condition), std::move(then_statements), std::move(else_statements));
	}

	ASTNode::Dependency CopyVisitor::clone(const nodes::LiteralNode& node)
	{
		return LiteralNode::create(node.value, node.literal_type);
	}

	ASTNode::Dependency CopyVisitor::clone(const nodes::ModuleNode& node)
	{
		return ModuleNode::create(node.name, clone(node.statements));
	}

	ASTNode::Dependency CopyVisitor::clone(const nodes::StructDeclarationNode& node)
	{
		return StructDeclarationNode::create(node.name, clone(node.parameters));
	}

	ASTNode::Dependency CopyVisitor::clone(const nodes::UnaryNode& node)
	{
		return UnaryNode::create(clone(node.expression.get()), node.op);
	}

	ASTNode::Dependency CopyVisitor::clone(const nodes::VariableDeclarationNode& node)
	{
		return VariableDeclarationNode::create(
			node.name, node.type_identifier, clone(node.expression.get()), node.is_mutable);
	}

	ASTNode::Dependency CopyVisitor::clone(const nodes::WhileNode& node)
	{
		auto condition{ clone(node.condition.get()) };
		auto statements{ clone(*static_cast<BlockNode*>(node.statements.get())) };
		return WhileNode::create(std::move(condition), std::move(statements));
	}
}  // namespace soul::ast::visitors
