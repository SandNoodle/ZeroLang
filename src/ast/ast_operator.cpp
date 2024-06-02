#include "ast/ast_operator.h"

#include <unordered_map>

namespace soul
{
	std::string_view ast_node_operator_to_string(ast_node_operator_t op)
	{
		static const std::unordered_map<ast_node_operator_t, std::string_view> k_operators = {
			{ ast_node_operator_t::operator_add,           "Add" },
			{ ast_node_operator_t::operator_subtract,      "Subtract" },
			{ ast_node_operator_t::operator_multiply,      "Multiply" },
			{ ast_node_operator_t::operator_divide,        "Divide" },
			{ ast_node_operator_t::operator_modulo,        "Modulo" },
			{ ast_node_operator_t::operator_increment,     "Increment" },
			{ ast_node_operator_t::operator_decrement,     "Decrement" },
			{ ast_node_operator_t::operator_equal,         "Equal" },
			{ ast_node_operator_t::operator_not_equal,     "NotEqual" },
			{ ast_node_operator_t::operator_greater,       "Greater" },
			{ ast_node_operator_t::operator_greater_equal, "GreaterEqual" },
			{ ast_node_operator_t::operator_less,          "Less" },
			{ ast_node_operator_t::operator_less_equal,    "LessEqual" },
			{ ast_node_operator_t::operator_logical_not,   "LogicalNot" },
			{ ast_node_operator_t::operator_logical_and,   "LogicalAnd" },
			{ ast_node_operator_t::operator_logical_or,    "LogicalOr" },
		};
		return k_operators.at(op);
	}
}  // namespace soul
