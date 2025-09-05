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

	ASTNodeOperator to_node_operator(Token::Type type)
	{
		static const std::unordered_map<Token::Type, ASTNodeOperator> k_operators = {
			{ Token::Type::SymbolPlus,               ASTNodeOperator::Add          },
			{ Token::Type::SymbolMinus,              ASTNodeOperator::Subtract     },
			{ Token::Type::SymbolStar,               ASTNodeOperator::Multiply     },
			{ Token::Type::SymbolSlash,              ASTNodeOperator::Divide       },
			{ Token::Type::SymbolPercent,            ASTNodeOperator::Modulo       },
			{ Token::Type::SymbolPlusPlus,           ASTNodeOperator::Increment    },
			{ Token::Type::SymbolMinusMinus,         ASTNodeOperator::Decrement    },
			{ Token::Type::SymbolEqual,              ASTNodeOperator::Equal        },
			{ Token::Type::SymbolBangEqual,          ASTNodeOperator::NotEqual     },
			{ Token::Type::SymbolGreater,            ASTNodeOperator::Greater      },
			{ Token::Type::SymbolGreaterEqual,       ASTNodeOperator::GreaterEqual },
			{ Token::Type::SymbolLess,               ASTNodeOperator::Less         },
			{ Token::Type::SymbolLessEqual,          ASTNodeOperator::LessEqual    },
			{ Token::Type::SymbolBang,               ASTNodeOperator::LogicalNot   },
			{ Token::Type::SymbolAmpersandAmpersand, ASTNodeOperator::LogicalAnd   },
			{ Token::Type::SymbolPipePipe,           ASTNodeOperator::LogicalOr    },
		};
		if (!k_operators.contains(type)) {
			return ASTNodeOperator::Unknown;
		}
		return k_operators.at(type);
	}
}  // namespace soul
