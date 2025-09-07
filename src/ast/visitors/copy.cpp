#include "ast/visitors/copy.h"

#include "ast/nodes/binary.h"
#include "ast/nodes/block.h"
#include "ast/nodes/cast.h"
#include "ast/nodes/error.h"
#include "ast/nodes/for_loop.h"
#include "ast/nodes/foreach_loop.h"
#include "ast/nodes/function_declaration.h"
#include "ast/nodes/if.h"
#include "ast/nodes/literal.h"
#include "ast/nodes/module.h"
#include "ast/nodes/struct_declaration.h"
#include "ast/nodes/unary.h"
#include "ast/nodes/variable_declaration.h"

#include <ranges>

namespace soul::ast::visitors
{
	using namespace ast::nodes;

	CopyVisitor::CopyVisitor() {}

	void CopyVisitor::accept(const ASTNode::Reference node)
	{
		if (!node) {
			return;
		}
		node->accept(*this);
	}

	void CopyVisitor::visit(const nodes::BinaryNode& node) { _cloned_root = clone(node); }
	void CopyVisitor::visit(const nodes::BlockNode& node) { _cloned_root = clone(node); }
	void CopyVisitor::visit(const nodes::CastNode& node) { _cloned_root = clone(node); }
	void CopyVisitor::visit(const nodes::ErrorNode& node) { _cloned_root = clone(node); }
	void CopyVisitor::visit(const nodes::ForLoopNode& node) { _cloned_root = clone(node); }
	void CopyVisitor::visit(const nodes::ForeachLoopNode& node) { _cloned_root = clone(node); }
	void CopyVisitor::visit(const nodes::FunctionDeclarationNode& node) { _cloned_root = clone(node); }
	void CopyVisitor::visit(const nodes::IfNode& node) { _cloned_root = clone(node); }
	void CopyVisitor::visit(const nodes::LiteralNode& node) { _cloned_root = clone(node); }
	void CopyVisitor::visit(const nodes::ModuleNode& node) { _cloned_root = clone(node); }
	void CopyVisitor::visit(const nodes::StructDeclarationNode& node) { _cloned_root = clone(node); }
	void CopyVisitor::visit(const nodes::UnaryNode& node) { _cloned_root = clone(node); }
	void CopyVisitor::visit(const nodes::VariableDeclarationNode& node) { _cloned_root = clone(node); }

	ASTNode::Dependency CopyVisitor::cloned() noexcept { return std::move(_cloned_root); }

	ASTNode::Dependency CopyVisitor::clone(const nodes::BinaryNode& node)
	{
		return BinaryNode::create(clone(node.lhs.get()), clone(node.rhs.get()), node.op);
	}

	ASTNode::ScopeBlock CopyVisitor::clone(const nodes::BlockNode& node)
	{
		auto cloned_statements{ node.statements
			                    | std::views::transform([this](const auto& s) { return clone(s.get()); })
			                    | std::ranges::to<ASTNode::Dependencies>() };
		return BlockNode::create(std::move(cloned_statements));
	}

	ASTNode::Dependency CopyVisitor::clone(const nodes::CastNode& node)
	{
		return CastNode::create(clone(node.expression.get()), node.type_identifier);
	}

	ASTNode::Dependency CopyVisitor::clone(const nodes::ErrorNode& node) { return ErrorNode::create(node.message); }

	ASTNode::Dependency CopyVisitor::clone(const nodes::ForLoopNode& node)
	{
		return ForLoopNode::create(clone(node.initialization.get()),
		                           clone(node.condition.get()),
		                           clone(node.update.get()),
		                           clone(*static_cast<BlockNode*>(node.statements.get())));
	}

	ASTNode::Dependency CopyVisitor::clone(const nodes::ForeachLoopNode& node)
	{
		return ForeachLoopNode::create(clone(node.variable.get()),
		                               clone(node.in_expression.get()),
		                               clone(*static_cast<BlockNode*>(node.statements.get())));
	}

	ASTNode::Dependency CopyVisitor::clone(const nodes::FunctionDeclarationNode& node)
	{
		auto cloned_parameters{ node.parameters
			                    | std::views::transform([this](const auto& p) { return clone(p.get()); })
			                    | std::ranges::to<ASTNode::Dependencies>() };
		return FunctionDeclarationNode::create(node.name,
		                                       node.return_type,
		                                       std::move(cloned_parameters),
		                                       clone(*static_cast<BlockNode*>(node.statements.get())));
	}

	ASTNode::Dependency CopyVisitor::clone(const nodes::IfNode& node)
	{
		return IfNode::create(clone(node.condition.get()),
		                      clone(*static_cast<BlockNode*>(node.if_statements.get())),
		                      clone(*static_cast<BlockNode*>(node.else_statements.get())));
	}

	ASTNode::Dependency CopyVisitor::clone(const nodes::LiteralNode& node) { return LiteralNode::create(node.value); }

	ASTNode::Dependency CopyVisitor::clone(const nodes::ModuleNode& node)
	{
		auto cloned_statements{ node.statements
			                    | std::views::transform([this](const auto& s) { return clone(s.get()); })
			                    | std::ranges::to<ASTNode::Dependencies>() };
		return ModuleNode::create(node.name, std::move(cloned_statements));
	}

	ASTNode::Dependency CopyVisitor::clone(const nodes::StructDeclarationNode& node)
	{
		auto cloned_parameters{ node.parameters
			                    | std::views::transform([this](const auto& p) { return clone(p.get()); })
			                    | std::ranges::to<ASTNode::Dependencies>() };
		return StructDeclarationNode::create(node.name, std::move(cloned_parameters));
	}

	ASTNode::Dependency CopyVisitor::clone(const nodes::UnaryNode& node)
	{
		return UnaryNode::create(clone(node.expr.get()), node.op);
	}

	ASTNode::Dependency CopyVisitor::clone(const nodes::VariableDeclarationNode& node)
	{
		return VariableDeclarationNode::create(
			node.name, node.type_identifier, clone(node.expr.get()), node.is_mutable);
	}

	ASTNode::Dependency CopyVisitor::clone(const ASTNode::Reference node)
	{
		if (!node) {
			return nullptr;
		}
		node->accept(*this);
		return std::move(_cloned_root);
	}
}  // namespace soul::ast::visitors
