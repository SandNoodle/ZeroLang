#pragma once

#include "common/types.h"

#include <string_view>

namespace soul
{
    /** Represents operator binding nodes, ex. binary, unary, ... */
    enum class ASTNodeOperator : u8
    {
        // Arithmetic
        OPERATOR_ADD,
        OPERATOR_SUBTRACT,
        OPERATOR_MULTIPLY,
        OPERATOR_DIVIDE,
        OPERATOR_MODULO,
        OPERATOR_INCREMENT,
        OPERATOR_DECREMENT,

        // Comparison
        OPERATOR_EQUAL,
        OPERATOR_NOT_EQUAL,
        OPERATOR_GREATER,
        OPERATOR_GREATER_EQUAL,
        OPERATOR_LESS,
        OPERATOR_LESS_EQUAL,

        // Logical
        OPERATOR_LOGICAL_NOT,
        OPERATOR_LOGICAL_AND,
        OPERATOR_LOGICAL_OR,
    };

	/** @brief Stringifies the AST Node's operator. */
    std::string_view ast_node_operator_to_string(ASTNodeOperator op);
}  // namespace soul