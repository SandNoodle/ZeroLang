#pragma once

#include "ast/nodes/nodes_fwd.h"

namespace soul::ast::visitors
{
	/**
	 * @brief Base class for visiting nodes in the Abstract Syntax Tree.
	 */
	class IVisitor
	{
		public:
		virtual ~IVisitor() = default;

		virtual void visit(nodes::AssignNode&);
		virtual void visit(nodes::BinaryNode&);
		virtual void visit(nodes::CastNode&);
		virtual void visit(nodes::ForLoopNode&);
		virtual void visit(nodes::ForeachLoopNode&);
		virtual void visit(nodes::FunctionDeclarationNode&);
		virtual void visit(nodes::IfNode&);
		virtual void visit(nodes::LiteralNode&);
		virtual void visit(nodes::ModuleNode&);
		virtual void visit(nodes::StructDeclarationNode&);
		virtual void visit(nodes::UnaryNode&);
		virtual void visit(nodes::VariableDeclarationNode&);

		virtual void visit(const nodes::AssignNode&);
		virtual void visit(const nodes::BinaryNode&);
		virtual void visit(const nodes::CastNode&);
		virtual void visit(const nodes::ForLoopNode&);
		virtual void visit(const nodes::ForeachLoopNode&);
		virtual void visit(const nodes::FunctionDeclarationNode&);
		virtual void visit(const nodes::IfNode&);
		virtual void visit(const nodes::LiteralNode&);
		virtual void visit(const nodes::ModuleNode&);
		virtual void visit(const nodes::StructDeclarationNode&);
		virtual void visit(const nodes::UnaryNode&);
		virtual void visit(const nodes::VariableDeclarationNode&);
	};
}  // namespace soul
