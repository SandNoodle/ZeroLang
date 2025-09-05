#pragma once

#include "ast/visitors/default_traverse.h"

namespace soul::ast::visitors
{
	/**
	 * @brief
	 */
	class CopyVisitor : public DefaultTraverseVisitor
	{
		private:
		ASTNode::Dependency _root{};

		public:
		CopyVisitor();

		/** @brief Returns */
		ASTNode::Dependency get() const;

		void accept(const ASTNode::Reference node) override;

		protected:
		using DefaultTraverseVisitor::visit;
	};
}  // namespace soul::ast::visitors
