#pragma once

#include <cstdint>
#include <memory>

namespace soul
{
	/**
	 * @brief
	 */
	class ast_node_t
	{
		public:
			using dependency_t = std::unique_ptr<ast_node_t>;

		public:
			virtual ~ast_node_t() = default;
	};
}
