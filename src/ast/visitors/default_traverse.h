#pragma once

#include "ast/ast.h"
#include "ast/nodes/nodes_fwd.h"
#include "ast/visitors/visitor.h"

#include <functional>

namespace soul
{
	/**
	 * @brief @TODO
	 */
	class DefaultTraverseVisitor : public IVisitor
	{
		public:
		virtual ~DefaultTraverseVisitor() = default;

		virtual void accept(const ASTNode::Reference node);

		virtual void visit(AssignNode&);
		virtual void visit(BinaryNode&);
		virtual void visit(ForLoopNode&);
		virtual void visit(ForeachLoopNode&);
		virtual void visit(FunctionDeclarationNode&);
		virtual void visit(IfNode&);
		virtual void visit(LiteralNode&);
		virtual void visit(ModuleNode&);
		virtual void visit(StructDeclarationNode&);
		virtual void visit(UnaryNode&);
		virtual void visit(VariableDeclarationNode&);

		virtual void visit(const AssignNode&);
		virtual void visit(const BinaryNode&);
		virtual void visit(const ForLoopNode&);
		virtual void visit(const ForeachLoopNode&);
		virtual void visit(const FunctionDeclarationNode&);
		virtual void visit(const IfNode&);
		virtual void visit(const LiteralNode&);
		virtual void visit(const ModuleNode&);
		virtual void visit(const StructDeclarationNode&);
		virtual void visit(const UnaryNode&);
		virtual void visit(const VariableDeclarationNode&);
	};
}  // namespace soul
