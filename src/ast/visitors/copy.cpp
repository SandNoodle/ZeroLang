#include "ast/visitors/copy.h"

namespace soul::ast::visitors
{
	using namespace ast::nodes;

	CopyVisitor::CopyVisitor() {}

	ASTNode::Dependency CopyVisitor::get() const { return _root->clone(); }

	void CopyVisitor::accept(const ASTNode::Reference node)
	{
		if (!node) {
			return;
		}
		_root = node->clone();
	}
}  // namespace soul::ast::visitors
