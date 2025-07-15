#include "common/types/primitive_type.h"

#include <unordered_map>

namespace soul::types
{

	PrimitiveType::operator std::string() const
	{
		using namespace std::string_view_literals;
		static constexpr auto                                            k_unknown = "__unknown__"sv;
		static std::unordered_map<PrimitiveType::Kind, std::string_view> k_types   = {
            { PrimitiveType::Kind::Unknown, k_unknown   },
            { PrimitiveType::Kind::Boolean, "bool"sv    },
            { PrimitiveType::Kind::Char,    "char"sv    },
            { PrimitiveType::Kind::Float32, "float32"sv },
            { PrimitiveType::Kind::Float64, "float64"sv },
            { PrimitiveType::Kind::Int32,   "int32"sv   },
            { PrimitiveType::Kind::Int64,   "int64"sv   },
            { PrimitiveType::Kind::String,  "string"sv  },
            { PrimitiveType::Kind::Void,    "void"sv    },
		};
		if (!k_types.contains(type)) [[unlikely]] {
			return std::string(k_unknown);
		}
		return std::string(k_types.at(type));
	}

	std::ostream& operator<<(std::ostream& os, const PrimitiveType& type) { return os << std::string(type); }
}  // namespace soul::types
