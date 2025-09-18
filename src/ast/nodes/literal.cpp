#include "ast/nodes/literal.h"

#include <unordered_map>

namespace soul::ast::nodes
{
	LiteralNode::LiteralNode(Value value, Type literal_type) : value(std::move(value)), literal_type(literal_type) {}

	LiteralNode::Dependency LiteralNode::create(Value value, Type literal_type)
	{
		return std::make_unique<LiteralNode>(std::move(value), literal_type);
	}

	LiteralNode::operator std::string() const noexcept { return std::string(value); }

	std::string_view LiteralNode::internal_name(const Type type) noexcept
	{
		using namespace std::string_view_literals;
		static constexpr auto                             k_unknown = "__unknown__"sv;
		static std::unordered_map<Type, std::string_view> k_types   = {
            { Type::Unknown,    k_unknown                   },
            { Type::Boolean,    "literal_type_boolean"sv    },
            { Type::Char,       "literal_type_char"sv       },
            { Type::Float32,    "literal_type_float32"sv    },
            { Type::Float64,    "literal_type_float64"sv    },
            { Type::Identifier, "literal_type_identifier"sv },
            { Type::Int32,      "literal_type_int32"sv      },
            { Type::Int64,      "literal_type_int64"sv      },
            { Type::String,     "literal_type_string"sv     },
		};
		if (!k_types.contains(type)) [[unlikely]] {
			return k_unknown;
		}
		return k_types.at(type);
	}
}  // namespace soul::ast::nodes
