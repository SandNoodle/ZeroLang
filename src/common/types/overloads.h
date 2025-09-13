#pragma once

#include "ast/ast.h"

#include <span>
#include <vector>

namespace soul::types
{
	/**
	 * @brief TODO
	 */
	struct OperatorOverload
	{
		ast::ASTNode::Operator   op;
		std::vector<types::Type> input_types;
		types::Type              return_type;

		static std::span<const OperatorOverload> all() noexcept;
	};

}  // namespace soul::types
