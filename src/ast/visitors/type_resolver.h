#pragma once

#include "ast/nodes/nodes_fwd.h"
#include "ast/visitors/copy.h"
#include "ast/visitors/type_discoverer.h"
#include "common/types/types_fwd.h"

#include <string_view>
#include <tuple>
#include <vector>

namespace soul::ast::visitors
{
	/**
	 * @brief TypeResolverVisitor traverses the AST while resolving each node into the correct type.
	 * @important Starting the visitor from nodes other than ModuleNode will result in modification of the input AST.
	 */
	class TypeResolverVisitor final : public CopyVisitor
	{
		public:
		enum Options : u8
		{
			None             = 1 << 0,
			ForceStrictCasts = 1 << 1,
		};

		using TypeMap = TypeDiscovererVisitor::TypeMap;

		private:
		using VariableContext = std::vector<std::pair<std::string_view, ASTNode::Reference>>;

		private:
		ASTNode::Reference _visited_node = nullptr;
		TypeMap            _registered_types;
		Options            _options;
		VariableContext    _variables_in_scope;

		public:
		TypeResolverVisitor(TypeMap type_map, Options options = Options::None);
		TypeResolverVisitor(const TypeResolverVisitor&)     = delete;
		TypeResolverVisitor(TypeResolverVisitor&&) noexcept = default;
		~TypeResolverVisitor()                              = default;

		TypeResolverVisitor& operator=(const TypeResolverVisitor&)     = delete;
		TypeResolverVisitor& operator=(TypeResolverVisitor&&) noexcept = default;

		using DefaultTraverseVisitor::accept;

		protected:
		using DefaultTraverseVisitor::visit;
		void visit(nodes::BinaryNode&) override;
		void visit(nodes::BlockNode&) override;
		void visit(nodes::CastNode&) override;
		void visit(nodes::ForLoopNode&) override;
		void visit(nodes::ForeachLoopNode&) override;
		void visit(nodes::FunctionDeclarationNode&) override;
		void visit(nodes::IfNode&) override;
		void visit(nodes::LiteralNode&) override;
		void visit(const nodes::ModuleNode&) override;
		void visit(nodes::StructDeclarationNode&) override;
		void visit(nodes::UnaryNode&) override;
		void visit(nodes::VariableDeclarationNode&) override;

		private:
		types::Type get_type_or_default(std::string_view type_identifier) const noexcept;
		bool        is_variable_declared(std::string_view name) const noexcept;
	};
}  // namespace soul::ast::visitors
