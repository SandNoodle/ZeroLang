#pragma once

#include "ast/visitors/default_traverse.h"
#include "core/types.h"

#include <limits>
#include <tuple>
#include <vector>

namespace soul::ast::visitors
{
	/**
	 * @brief ErrorPropagationVisitor traverses the AST while gathering info of any `ErrorNode`s that might be present.
	 * @details Abstract Syntax Tree is well formed and semantically correct if it does not contain any `ErrorNode`s.
	 */
	class ErrorCollectorVisitor : public DefaultTraverseVisitor
	{
		public:
		using Errors = std::vector<std::pair<std::size_t, const nodes::ErrorNode*>>;

		static constexpr auto k_depth_max = std::numeric_limits<std::size_t>::max();

		private:
		std::size_t _depth_current;
		std::size_t _depth_max;
		Errors      _errors;

		public:
		ErrorCollectorVisitor(std::size_t max_depth = k_depth_max);

		void accept(ASTNode::Reference node) override;

		/**
		 * @brief Returns true if the AST does not contain any error nodes, i.e. is well formed and semantically
		 * correct.
		 */
		bool is_valid() const noexcept;

		/**
		 * @brief Returns all errors gathered (up to maximum depth) during AST traversal.
		 */
		const Errors& errors() const noexcept;

		private:
		using DefaultTraverseVisitor::visit;
		void visit(const nodes::ErrorNode&) override;
	};
}  // namespace soul::ast::visitors
