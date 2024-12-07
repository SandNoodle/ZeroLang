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
		bool _print_types;

		public:
		StringifyVisitor(bool print_types = true);

		std::string string() const;

		void visit(const AssignNode& node) override;
		void visit(const BinaryNode& node) override;
		void visit(const BlockNode& node) override;
		void visit(const FunctionDeclarationNode& node) override;
		void visit(const LiteralNode& node) override;
		void visit(const StructDeclarationNode& node) override;
		void visit(const UnaryNode& node) override;
		void visit(const VariableDeclarationNode& node) override;
	};
}  // namespace soul
