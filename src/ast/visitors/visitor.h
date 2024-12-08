#pragma once

#include "ast/nodes/nodes_fwd.h"

namespace soul
{
	/**
	 * @brief Base class for visiting nodes in the Abstract Syntax Tree.
	 */
	class IVisitor
	{
		public:
		virtual ~IVisitor() = default;

		virtual void visit(AssignNode&);
		virtual void visit(BinaryNode&);
		virtual void visit(BlockNode&);
		virtual void visit(ForLoopNode&);
		virtual void visit(ForeachLoopNode&);
		virtual void visit(FunctionDeclarationNode&);
		virtual void visit(IfNode&);
		virtual void visit(LiteralNode&);
		virtual void visit(StructDeclarationNode&);
		virtual void visit(UnaryNode&);
		virtual void visit(VariableDeclarationNode&);

		virtual void visit(const AssignNode&);
		virtual void visit(const BinaryNode&);
		virtual void visit(const BlockNode&);
		virtual void visit(const ForLoopNode&);
		virtual void visit(const ForeachLoopNode&);
		virtual void visit(const FunctionDeclarationNode&);
		virtual void visit(const IfNode&);
		virtual void visit(const LiteralNode&);
		virtual void visit(const StructDeclarationNode&);
		virtual void visit(const UnaryNode&);
		virtual void visit(const VariableDeclarationNode&);
	};
}  // namespace soul
