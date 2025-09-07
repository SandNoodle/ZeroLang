#pragma once

#include "ast/nodes/nodes_fwd.h"
#include "ast/visitors/default_traverse.h"

#include <cstddef>
#include <sstream>

namespace soul::ast::visitors
{
	/**
	 * @brief
	 */
	class StringifyVisitor final : public DefaultTraverseVisitor
	{
		private:
		std::stringstream _ss;

		public:
		StringifyVisitor();

		std::string string() const;

		void accept(const ASTNode::Reference node) override;

		protected:
		using DefaultTraverseVisitor::visit;
		void visit(const nodes::BinaryNode&) override;
		void visit(const nodes::BlockNode&) override;
		void visit(const nodes::CastNode&) override;
		void visit(const nodes::ErrorNode&) override;
		void visit(const nodes::ForLoopNode&) override;
		void visit(const nodes::ForeachLoopNode&) override;
		void visit(const nodes::FunctionDeclarationNode&) override;
		void visit(const nodes::IfNode&) override;
		void visit(const nodes::LiteralNode&) override;
		void visit(const nodes::ModuleNode&) override;
		void visit(const nodes::StructDeclarationNode&) override;
		void visit(const nodes::UnaryNode&) override;
		void visit(const nodes::VariableDeclarationNode&) override;
	};
}  // namespace soul::ast::visitors
