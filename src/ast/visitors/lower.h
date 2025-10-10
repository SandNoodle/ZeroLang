#pragma once

#include "ast/visitors/default_traverse.h"

#include "ir/builder.h"
#include "ir/instruction.h"
#include "ir/ir.h"

#include <vector>

namespace soul::ast::visitors
{
	/**
	 * @brief CodegenVisitor lowers the AST into langauge's IR.
	 * It is the last visitor that performs traversal on the AST.
	 */
	class LowerVisitor : public DefaultTraverseVisitor
	{
		private:
		ir::IRBuilder    _builder{};
		ir::Instruction* _current_instruction = nullptr;

		public:
		/** @brief Returns IR representation equivalent to the AST. */
		std::unique_ptr<ir::Module> get() noexcept;

		using DefaultTraverseVisitor::accept;

		private:
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

		/**
		 * @brief Visits the specified node and emits its equivalent instruction(s).
		 * @warning Some nodes may not be visited directly and should be handled separately (see implementation).
		 */
		ir::Instruction* emit(const ASTNode::Reference);
		ir::Instruction* emit(const nodes::BinaryNode&);
		ir::Instruction* emit(const nodes::BlockNode&);
		ir::Instruction* emit(const nodes::CastNode&);
		ir::Instruction* emit(const nodes::ErrorNode&);
		ir::Instruction* emit(const nodes::ForLoopNode&);
		ir::Instruction* emit(const nodes::ForeachLoopNode&);
		ir::Instruction* emit(const nodes::FunctionCallNode&);
		ir::Instruction* emit(const nodes::FunctionDeclarationNode&);
		ir::Instruction* emit(const nodes::IfNode&);
		ir::Instruction* emit(const nodes::LiteralNode&);
		ir::Instruction* emit(const nodes::ModuleNode&);
		ir::Instruction* emit(const nodes::StructDeclarationNode&);
		ir::Instruction* emit(const nodes::UnaryNode&);
		ir::Instruction* emit(const nodes::VariableDeclarationNode&);
		ir::Instruction* emit(const nodes::WhileNode&);
	};
}  // namespace soul::ast::visitors
