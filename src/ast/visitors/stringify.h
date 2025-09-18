#pragma once

#include "ast/nodes/nodes_fwd.h"
#include "ast/visitors/default_traverse.h"

#include <cstddef>
#include <sstream>

namespace soul::ast::visitors
{
	/**
	 * @brief StringifyVisitor traverses the AST and converts each visited node into its (valid) JSON representation.
	 * Its useful for debugging.
	 */
	class StringifyVisitor final : public DefaultTraverseVisitor
	{
		private:
		static constexpr auto        k_unnamed       = "__unnamed__";
		static constexpr std::size_t k_indent_amount = 2;

		private:
		std::stringstream _ss;
		std::size_t       _indent_level = 0;

		public:
		StringifyVisitor();

		/** @brief Returns textual representation of an AST. */
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
		void visit(const nodes::FunctionCallNode&) override;
		void visit(const nodes::FunctionDeclarationNode&) override;
		void visit(const nodes::IfNode&) override;
		void visit(const nodes::LiteralNode&) override;
		void visit(const nodes::ModuleNode&) override;
		void visit(const nodes::StructDeclarationNode&) override;
		void visit(const nodes::UnaryNode&) override;
		void visit(const nodes::VariableDeclarationNode&) override;

		private:
		std::string current_indent() const;
		void        encode(std::string_view key, std::string_view value, bool add_trailing_comma = true);
		void        encode(std::string_view key, const ASTNode::Reference node, bool add_trailing_comma = true);
		void        encode(std::string_view key, const ASTNode::References& parameters, bool add_trailing_comma = true);
	};
}  // namespace soul::ast::visitors
