#include "ast/visitors/desugar.h"

#include "ast/nodes/binary.h"
#include "ast/nodes/error.h"
#include "ast/nodes/for_loop.h"
#include "ast/nodes/foreach_loop.h"
#include "ast/nodes/literal.h"
#include "ast/nodes/unary.h"
#include "ast/nodes/while.h"

namespace soul::ast::visitors
{
	using namespace ast::nodes;

	void DesugarVisitor::visit(const ForLoopNode& node)
	{
		// ForLoopNode(initialization, condition, update, BlockNode({...}))
		// ...is equivalent to...
		// BlockNode({initialization, WhileNode(condition, BlockNode({..., update}))})

		CopyVisitor::visit(node);
		auto& for_loop = as<ForLoopNode>();

		// Construct WhileNode's body with <update> appended at the end.
		ASTNode::Dependencies while_body_statements{};
		while_body_statements.reserve(for_loop.statements->statements.size() + (for_loop.update ? 1 : 0));
		while_body_statements.insert(while_body_statements.end(),
		                             std::make_move_iterator(for_loop.statements->statements.begin()),
		                             std::make_move_iterator(for_loop.statements->statements.end()));
		if (for_loop.update) {
			while_body_statements.push_back(std::move(for_loop.update));
		}

		// Hoist <initialization> before WhileNode.
		ASTNode::Dependencies statements{};
		statements.reserve(2);
		if (for_loop.initialization) {
			statements.push_back(std::move(for_loop.initialization));
		}
		statements.emplace_back(
			WhileNode::create(std::move(for_loop.condition), BlockNode::create(std::move(while_body_statements))));

		_current_clone = BlockNode::create(std::move(statements));
	}
}  // namespace soul::ast::visitors
