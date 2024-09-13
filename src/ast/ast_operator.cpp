#include "ast/ast_operator.h"

#include <unordered_map>

namespace soul
{
	std::string_view ast_node_operator_to_string(ASTNodeOperator op)
	{
		static const std::unordered_map<ASTNodeOperator, std::string_view> k_operators = {
			{ ASTNodeOperator::OPERATOR_ADD,           "Add"          },
			{ ASTNodeOperator::OPERATOR_SUBTRACT,      "Subtract"     },
			{ ASTNodeOperator::OPERATOR_MULTIPLY,      "Multiply"     },
			{ ASTNodeOperator::OPERATOR_DIVIDE,        "Divide"       },
			{ ASTNodeOperator::OPERATOR_MODULO,        "Modulo"       },
			{ ASTNodeOperator::OPERATOR_INCREMENT,     "Increment"    },
			{ ASTNodeOperator::OPERATOR_DECREMENT,     "Decrement"    },
			{ ASTNodeOperator::OPERATOR_EQUAL,         "Equal"        },
			{ ASTNodeOperator::OPERATOR_NOT_EQUAL,     "NotEqual"     },
			{ ASTNodeOperator::OPERATOR_GREATER,       "Greater"      },
			{ ASTNodeOperator::OPERATOR_GREATER_EQUAL, "GreaterEqual" },
			{ ASTNodeOperator::OPERATOR_LESS,          "Less"         },
			{ ASTNodeOperator::OPERATOR_LESS_EQUAL,    "LessEqual"    },
			{ ASTNodeOperator::OPERATOR_LOGICAL_NOT,   "LogicalNot"   },
			{ ASTNodeOperator::OPERATOR_LOGICAL_AND,   "LogicalAnd"   },
			{ ASTNodeOperator::OPERATOR_LOGICAL_OR,    "LogicalOr"    },
		};
		return k_operators.at(op);
	}
}  // namespace soul
