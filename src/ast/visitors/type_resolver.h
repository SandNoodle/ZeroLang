#pragma once

#include "ast/nodes/nodes_fwd.h"
#include "common/types/types_fwd.h"
#include "ast/visitors/default_traverse.h"
#include "common/diagnostic.h"

#include <string>
#include <unordered_map>

namespace soul::ast::visitors
{

	class TypeResolverVisitor : public DefaultTraverseVisitor
	{
		public:
		enum ResolveFlags : u8
		{
			None             = 1 << 0,
			ForceStrictCasts = 1 << 1,
		};

		private:
		ResolveFlags                                 _flags;
		std::unordered_map<std::string, types::Type> _registered_types;
		mutable Diagnostics                          _diagnostics;

		public:
		TypeResolverVisitor(ResolveFlags flags = ResolveFlags::None);
		TypeResolverVisitor(const TypeResolverVisitor&)     = delete;
		TypeResolverVisitor(TypeResolverVisitor&&) noexcept = default;
		~TypeResolverVisitor()                              = default;

		TypeResolverVisitor& operator=(const TypeResolverVisitor&)     = delete;
		TypeResolverVisitor& operator=(TypeResolverVisitor&&) noexcept = default;

		using DefaultTraverseVisitor::accept;
		using DefaultTraverseVisitor::visit;
		void visit(nodes::AssignNode&) override;
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

		private:
		bool register_type(const std::string& name, types::Type&& type);
		void register_basic_types();

		/**
		 * @brief Creates diagnostic message.
		 * @param code Diagnostic code - determines the message.
		 * @param args Arguments to format the diagnostic string with.
		 */
		template <typename... Args>
		void diagnostic(DiagnosticType type, DiagnosticCode code, Args&&... args) const
		{
			_diagnostics.emplace_back(type, code, std::forward<Args>(args)...);
		}
	};
}  // namespace soul::ast::visitors
