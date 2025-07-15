#pragma once

#include "ast/nodes/nodes_fwd.h"
#include "ast/visitors/default_traverse.h"
#include "common/types/types_fwd.h"

#include <string_view>
#include <unordered_map>

namespace soul::ast::visitors
{
	/**
	 * @brief TypeDiscovererVisitor traverses
	 */
	class TypeDiscovererVisitor : public DefaultTraverseVisitor
	{
		public:
		using TypeMap = std::unordered_map<std::string_view, types::Type>;

		private:
		TypeMap _registered_types;

		public:
		TypeDiscovererVisitor();
		TypeDiscovererVisitor(const TypeDiscovererVisitor&)     = delete;
		TypeDiscovererVisitor(TypeDiscovererVisitor&&) noexcept = default;
		~TypeDiscovererVisitor()                                = default;

		TypeDiscovererVisitor& operator=(const TypeDiscovererVisitor&)     = delete;
		TypeDiscovererVisitor& operator=(TypeDiscovererVisitor&&) noexcept = default;

		/** @brief */
		TypeMap get() noexcept;

		using DefaultTraverseVisitor::accept;
		using DefaultTraverseVisitor::visit;
		void visit(const nodes::StructDeclarationNode&) override;

		private:
		void register_basic_types();
	};
}  // namespace soul::ast::visitors
