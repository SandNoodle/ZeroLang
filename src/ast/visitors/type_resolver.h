#pragma once

#include "ast/nodes/nodes_fwd.h"
#include "ast/visitors/copy.h"
#include "ast/visitors/type_discoverer.h"
#include "common/types/types_fwd.h"

#include <optional>
#include <ranges>
#include <string_view>
#include <tuple>
#include <vector>

namespace soul::ast::visitors
{
	/**
	 * @brief TypeResolverVisitor traverses the AST while resolving each node into the correct type.
	 */
	class TypeResolverVisitor final : public CopyVisitor
	{
		public:
		using TypeMap = TypeDiscovererVisitor::TypeMap;

		private:
		struct FunctionDeclaration
		{
			std::vector<types::Type> input_types;
			types::Type              return_type;
		};

		using VariableContext = std::vector<std::pair<std::string, types::Type>>;
		using FunctionContext = std::vector<std::pair<std::string, FunctionDeclaration>>;

		private:
		TypeMap         _registered_types;
		VariableContext _variables_in_scope;
		FunctionContext _functions_in_module;

		public:
		TypeResolverVisitor(TypeMap type_map);
		TypeResolverVisitor(const TypeResolverVisitor&)     = delete;
		TypeResolverVisitor(TypeResolverVisitor&&) noexcept = default;
		~TypeResolverVisitor()                              = default;

		TypeResolverVisitor& operator=(const TypeResolverVisitor&)     = delete;
		TypeResolverVisitor& operator=(TypeResolverVisitor&&) noexcept = default;

		using CopyVisitor::accept;

		protected:
		using CopyVisitor::visit;
		void visit(const nodes::BinaryNode&) override;
		void visit(const nodes::BlockNode&) override;
		void visit(const nodes::CastNode&) override;
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

		private:
		types::Type                        get_type_or_default(std::string_view type_identifier) const noexcept;
		std::optional<types::Type>         get_variable_type(std::string_view name) const noexcept;
		types::Type                        get_type_for_operator(ASTNode::Operator                      op,
		                                                         const std::ranges::forward_range auto& input_types) const noexcept;
		std::optional<FunctionDeclaration> get_function_declaration(
			std::string_view                       name,
			const std::ranges::forward_range auto& want_types) const noexcept;
	};
}  // namespace soul::ast::visitors
#include "ast/visitors/type_resolver.inl"
