#include "ast/nodes/block.h"

namespace soul
{
	block_t::block_t(std::vector<ast_node_t::dependency_t>&& statements)
		: _statements(std::move(statements)) {}

	block_t::dependency_t block_t::create(std::vector<ast_node_t::dependency_t>&& statements)
	{
		return std::make_unique<block_t>(std::move(statements));
	}
}  // namespace soul

