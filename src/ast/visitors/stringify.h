#pragma once

#include "ast/nodes/nodes_fwd.h"
#include "ast/visitors/default_traverse.h"

#include <cstddef>
#include <sstream>

namespace soul
{
	class StringifyVisitor final : public DefaultTraverseVisitor
	{
		private:
		std::stringstream _ss;
		std::size_t       _indent = 0;

		public:
		StringifyVisitor();

		std::string string() const;

		using DefaultTraverseVisitor::visit;
		void accept(const ASTNode::Reference node) override;
		void visit(const AssignNode&) override;
		void visit(const BinaryNode&) override;
		void visit(const ForLoopNode&) override;
		void visit(const ForeachLoopNode&) override;
		void visit(const FunctionDeclarationNode&) override;
		void visit(const IfNode&) override;
		void visit(const LiteralNode&) override;
		void visit(const ModuleNode&) override;
		void visit(const StructDeclarationNode&) override;
		void visit(const UnaryNode&) override;
		void visit(const VariableDeclarationNode&) override;
	};
}  // namespace soul
