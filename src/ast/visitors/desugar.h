#pragma once

#include "ast/nodes/nodes_fwd.h"
#include "ast/visitors/copy.h"
#include "common/types/types_fwd.h"

namespace soul::ast::visitors
{
	/**
	 * @brief DesugarVisitor traverse the AST while substituting high-level nodes (such as ForNode or ForeachNode)
	 * into a lower-level ones (such as WhileNode).
	 */
	class DesugarVisitor : public CopyVisitor
	{
		public:
		using CopyVisitor::accept;

		protected:
		using CopyVisitor::visit;
		void visit(const nodes::BinaryNode&) override;
		void visit(const nodes::ForLoopNode&) override;
	};
}  // namespace soul::ast::visitors
