#include "ast/nodes/binary.h"

namespace soul
{
	binary_t::binary_t(dependency_t lhs, dependency_t rhs, ast_node_operator_t op)
	: _lhs(std::move(lhs)), _rhs(std::move(rhs)), _operator(op) {}

	binary_t::dependency_t binary_t::create(dependency_t lhs, dependency_t rhs, ast_node_operator_t op)
	{
		return std::make_unique<binary_t>(std::move(lhs), std::move(rhs), op);
	}
}  // namespace soul

