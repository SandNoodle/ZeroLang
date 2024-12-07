#pragma once

#include "core/types.h"

#include <string_view>

namespace soul
{
	enum class TokenType : u8;

	/** Represents operator binding nodes, ex. binary, unary, ... */
	enum class ASTNodeOperator : u8
	{
		Unknown,  // Yet to be resolved or an error value.

		// Arithmetic
		Add,
		Subtract,
		Multiply,
		Divide,
		Modulo,
		Increment,
		Decrement,

		// Comparison
		Equal,
		NotEqual,
		Greater,
		GreaterEqual,
		Less,
		LessEqual,

		// Logical
		LogicalNot,
		LogicalAnd,
		LogicalOr,
	};
	ASTNodeOperator to_node_operator(TokenType);
	std::string_view to_string(ASTNodeOperator);

}  // namespace soul
