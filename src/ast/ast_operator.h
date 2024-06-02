#pragma once

#include <cstdint>
#include <string_view>

namespace soul
{
    /** Represents operator binding nodes, ex. binary, unary, ... */
    enum class ast_node_operator_t : uint8_t
    {
        // Arithmetic
        operator_add,
        operator_subtract,
        operator_multiply,
        operator_divide,
        operator_modulo,
        operator_increment,
        operator_decrement,

        // Comparison
        operator_equal,
        operator_not_equal,
        operator_greater,
        operator_greater_equal,
        operator_less,
        operator_less_equal,

        // Logical
        operator_logical_not,
        operator_logical_and,
        operator_logical_or,
    };

    std::string_view ast_node_operator_to_string(ast_node_operator_t op);
}  // namespace soul