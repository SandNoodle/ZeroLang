#include "ast/nodes/unary.h"

namespace soul
{
	unary_t::unary_t(dependency_t expr, ast_node_operator_t op)
		: _expr(std::move(expr)), _operator(op) {}

	unary_t::dependency_t unary_t::create(dependency_t expr, ast_node_operator_t op)
	{
		return std::make_unique<unary_t>(std::move(expr), op);
	}
}  // namespace soul
