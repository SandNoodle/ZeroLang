#pragma once

#include "ast/nodes/nodes_fwd.h"
#include "ast/visitors/copy.h"
#include "common/types/types_fwd.h"

#include <string_view>
#include <unordered_map>

namespace soul::ast::visitors
{
	/**
	 * @brief TypeDiscovererVisitor traverses
	 */
	class TypeDiscovererVisitor : public CopyVisitor
	{
		public:
		using TypeMap = std::unordered_map<std::string_view, types::Type>;

		private:
		TypeMap _registered_types = basic_types();

		public:
		TypeDiscovererVisitor()                                 = default;
		TypeDiscovererVisitor(const TypeDiscovererVisitor&)     = delete;
		TypeDiscovererVisitor(TypeDiscovererVisitor&&) noexcept = default;
		~TypeDiscovererVisitor()                                = default;

		TypeDiscovererVisitor& operator=(const TypeDiscovererVisitor&)     = delete;
		TypeDiscovererVisitor& operator=(TypeDiscovererVisitor&&) noexcept = default;

		/** @brief */
		TypeMap get() noexcept;

		using CopyVisitor::accept;

		static TypeMap basic_types() noexcept;

		protected:
		using CopyVisitor::visit;
		void visit(nodes::StructDeclarationNode&) override;
	};
}  // namespace soul::ast::visitors
