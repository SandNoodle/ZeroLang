#pragma once

#include "ast/nodes/nodes_fwd.h"
#include "ast/types/types_fwd.h"
#include "ast/visitors/default_traverse.h"

#include <string>
#include <unordered_map>

namespace soul::ast::visitors
{
	class TypeResolverVisitor : public DefaultTraverseVisitor
	{
		private:
		std::unordered_map<std::string, types::Type> _registered_types;

		public:
		TypeResolverVisitor();
		TypeResolverVisitor(const TypeResolverVisitor&)     = delete;
		TypeResolverVisitor(TypeResolverVisitor&&) noexcept = default;
		~TypeResolverVisitor()                              = default;

		TypeResolverVisitor& operator=(const TypeResolverVisitor&)     = delete;
		TypeResolverVisitor& operator=(TypeResolverVisitor&&) noexcept = default;

		using DefaultTraverseVisitor::visit;
		void visit(nodes::AssignNode&) override;
		void visit(nodes::BinaryNode&) override;
		void visit(nodes::ForLoopNode&) override;
		void visit(nodes::ForeachLoopNode&) override;
		void visit(nodes::FunctionDeclarationNode&) override;
		void visit(nodes::IfNode&) override;
		void visit(nodes::LiteralNode&) override;
		void visit(nodes::ModuleNode&) override;
		void visit(nodes::StructDeclarationNode&) override;
		void visit(nodes::UnaryNode&) override;
		void visit(nodes::VariableDeclarationNode&) override;

		private:
		void resolve(ASTNode::Reference node);

		bool register_type(const std::string& name, types::Type&& type);

		void register_basic_types();
	};
}  // namespace soul::ast::visitors
