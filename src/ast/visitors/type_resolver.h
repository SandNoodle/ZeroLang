#pragma once

#include "ast/nodes/nodes_fwd.h"
#include "ast/visitors/default_traverse.h"
#include "ast/visitors/type_discoverer.h"
#include "common/types/types_fwd.h"

#include <optional>
#include <ranges>
#include <string_view>
#include <unordered_map>

namespace soul::ast::visitors
{
	/** @brief */
	enum class CastType : u8
	{
		Implicit,
		Explicit,
		Impossible,
	};

	/**
	 * @brief
	 */
	class TypeResolverVisitor : public DefaultTraverseVisitor
	{
		public:
		enum ResolveFlags : u8
		{
			None             = 1 << 0,
			ForceStrictCasts = 1 << 1,
		};

		using TypeMap = TypeDiscovererVisitor::TypeMap;

		private:
		TypeMap      _registered_types;
		ResolveFlags _flags;

		public:
		TypeResolverVisitor(TypeMap type_map, ResolveFlags flags = ResolveFlags::None);
		TypeResolverVisitor(const TypeResolverVisitor&)     = delete;
		TypeResolverVisitor(TypeResolverVisitor&&) noexcept = default;
		~TypeResolverVisitor()                              = default;

		TypeResolverVisitor& operator=(const TypeResolverVisitor&)     = delete;
		TypeResolverVisitor& operator=(TypeResolverVisitor&&) noexcept = default;

		using DefaultTraverseVisitor::accept;
		using DefaultTraverseVisitor::visit;
		void visit(nodes::BinaryNode&) override;
		void visit(nodes::CastNode&) override;
		void visit(nodes::ForLoopNode&) override;
		void visit(nodes::ForeachLoopNode&) override;
		void visit(nodes::FunctionDeclarationNode&) override;
		void visit(nodes::IfNode&) override;
		void visit(nodes::LiteralNode&) override;
		void visit(nodes::ModuleNode&) override;
		void visit(nodes::StructDeclarationNode&) override;
		void visit(nodes::UnaryNode&) override;
		void visit(nodes::VariableDeclarationNode&) override;
	};
}  // namespace soul::ast::visitors
