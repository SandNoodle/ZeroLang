#pragma once

#include "ast/ast.h"

#include <vector>

namespace soul
{
	/**
	 * @brief Represents a 'Block' of statements
	 * in the Abstract Syntax Tree (AST).
	 */
	class block_t final : public ast_node_acceptor_t<block_t>
	{
		public:
			using dependency_t = ast_node_t::dependency_t;
			using dependencies_t = ast_node_t::dependencies_t;

		private:
			dependencies_t _statements{};

		public:
			explicit block_t(std::vector<ast_node_t::dependency_t>&& statements);
			~block_t() override = default;

			/**
			 * @brief Constructs new Block statement node.
			 * @param statements statements that are a part of this block.
			 * @return New 'Block' statement node.
			 */
			static dependency_t create(std::vector<ast_node_t::dependency_t>&& statements);
	};
}  // namespace soul
