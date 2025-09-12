#include "ast/ast.h"

#include <unordered_map>

namespace soul::ast
{
	std::string_view ASTNode::name(const ASTNode::Operator op) noexcept
	{
		using namespace std::string_view_literals;
		static const std::unordered_map<ASTNode::Operator, std::string_view> k_operators = {
			{ Operator::Unknown,      "__unknown__"sv },
			{ Operator::Add,          "+"sv           },
			{ Operator::Sub,          "-"sv           },
			{ Operator::Mul,          "*"sv           },
			{ Operator::Div,          "/"sv           },
			{ Operator::Mod,          "%"sv           },
			{ Operator::Increment,    "++"sv          },
			{ Operator::Decrement,    "--"sv          },
			{ Operator::Equal,        "="sv           },
			{ Operator::NotEqual,     "!="sv          },
			{ Operator::Greater,      ">"sv           },
			{ Operator::GreaterEqual, ">="sv          },
			{ Operator::Less,         "<"sv           },
			{ Operator::LessEqual,    "<="sv          },
			{ Operator::LogicalNot,   "!"sv           },
			{ Operator::LogicalAnd,   "&&"sv          },
			{ Operator::LogicalOr,    "||"sv          },
		};
		if (!k_operators.contains(op)) [[unlikely]] {
			return k_operators.at(Operator::Unknown);
		}
		return k_operators.at(op);
	}

	ASTNode::Operator ASTNode::as_operator(Token::Type type) noexcept
	{
		static const std::unordered_map<Token::Type, Operator> k_operators = {
			{ Token::Type::SymbolPlus,               Operator::Add          },
			{ Token::Type::SymbolMinus,              Operator::Sub          },
			{ Token::Type::SymbolStar,               Operator::Mul          },
			{ Token::Type::SymbolSlash,              Operator::Div          },
			{ Token::Type::SymbolPercent,            Operator::Mod          },
			{ Token::Type::SymbolPlusPlus,           Operator::Increment    },
			{ Token::Type::SymbolMinusMinus,         Operator::Decrement    },
			{ Token::Type::SymbolEqual,              Operator::Equal        },
			{ Token::Type::SymbolBangEqual,          Operator::NotEqual     },
			{ Token::Type::SymbolGreater,            Operator::Greater      },
			{ Token::Type::SymbolGreaterEqual,       Operator::GreaterEqual },
			{ Token::Type::SymbolLess,               Operator::Less         },
			{ Token::Type::SymbolLessEqual,          Operator::LessEqual    },
			{ Token::Type::SymbolBang,               Operator::LogicalNot   },
			{ Token::Type::SymbolAmpersandAmpersand, Operator::LogicalAnd   },
			{ Token::Type::SymbolPipePipe,           Operator::LogicalOr    },
		};
		if (!k_operators.contains(type)) {
			return Operator::Unknown;
		}
		return k_operators.at(type);
	}
}  // namespace soul::ast
