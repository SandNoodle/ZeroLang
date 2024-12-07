#include "ast/ast_operator.h"

#include "lexer/token.h"

#include <unordered_map>

namespace soul
{
	std::string_view to_string(ASTNodeOperator op)
	{
		using namespace std::string_view_literals;
		static const std::unordered_map<ASTNodeOperator, std::string_view> k_operators = {
			{ ASTNodeOperator::Unknown,      "Unknown"sv      },
			{ ASTNodeOperator::Add,          "Add"sv          },
			{ ASTNodeOperator::Subtract,     "Subtract"sv     },
			{ ASTNodeOperator::Multiply,     "Multiply"sv     },
			{ ASTNodeOperator::Divide,       "Divide"sv       },
			{ ASTNodeOperator::Modulo,       "Modulo"sv       },
			{ ASTNodeOperator::Increment,    "Increment"sv    },
			{ ASTNodeOperator::Decrement,    "Decrement"sv    },
			{ ASTNodeOperator::Equal,        "Equal"sv        },
			{ ASTNodeOperator::NotEqual,     "NotEqual"sv     },
			{ ASTNodeOperator::Greater,      "Greater"sv      },
			{ ASTNodeOperator::GreaterEqual, "GreaterEqual"sv },
			{ ASTNodeOperator::Less,         "Less"sv         },
			{ ASTNodeOperator::LessEqual,    "LessEqual"sv    },
			{ ASTNodeOperator::LogicalNot,   "LogicalNot"sv   },
			{ ASTNodeOperator::LogicalAnd,   "LogicalAnd"sv   },
			{ ASTNodeOperator::LogicalOr,    "LogicalOr"sv    },
		};
		return k_operators.at(op);
	}

	ASTNodeOperator to_node_operator(TokenType type)
	{
		static const std::unordered_map<TokenType, ASTNodeOperator> k_operators = {
			{ TokenType::Plus,            ASTNodeOperator::Add          },
			{ TokenType::Minus,           ASTNodeOperator::Subtract     },
			{ TokenType::Star,            ASTNodeOperator::Multiply     },
			{ TokenType::Slash,           ASTNodeOperator::Divide       },
			{ TokenType::Percent,         ASTNodeOperator::Modulo       },
			{ TokenType::DoublePlus,      ASTNodeOperator::Increment    },
			{ TokenType::DoubleMinus,     ASTNodeOperator::Decrement    },
			{ TokenType::Equal,           ASTNodeOperator::Equal        },
			{ TokenType::BangEqual,       ASTNodeOperator::NotEqual     },
			{ TokenType::Greater,         ASTNodeOperator::Greater      },
			{ TokenType::GreaterEqual,    ASTNodeOperator::GreaterEqual },
			{ TokenType::Less,            ASTNodeOperator::Less         },
			{ TokenType::LessEqual,       ASTNodeOperator::LessEqual    },
			{ TokenType::Bang,            ASTNodeOperator::LogicalNot   },
			{ TokenType::DoubleAmpersand, ASTNodeOperator::LogicalAnd   },
			{ TokenType::DoublePipe,      ASTNodeOperator::LogicalOr    },
		};
		if (!k_operators.contains(type)) {
			return ASTNodeOperator::Unknown;
		}
		return k_operators.at(type);
	}
}  // namespace soul
