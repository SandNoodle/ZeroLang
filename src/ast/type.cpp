#include "ast/type.h"

#include <sstream>
#include <unordered_map>

namespace soul
{
	//
	// Array
	//

	ArrayType::ArrayType(const ArrayType& other) noexcept
		: _contained_type(std::make_unique<Type>(*other._contained_type.get()))
	{
	}

	ArrayType::ArrayType(ArrayType&& other) noexcept
		: _contained_type(std::make_unique<Type>(*other._contained_type.get()))
	{
	}

	ArrayType::ArrayType(Type contained_type) : _contained_type(std::make_unique<Type>(std::move(contained_type))) {}

	ArrayType& ArrayType::operator=(const ArrayType& other) noexcept
	{
		if (this == &other) return *this;

		this->_contained_type = std::make_unique<Type>(*other._contained_type.get());
		return *this;
	}

	ArrayType& ArrayType::operator=(ArrayType&& other) noexcept
	{
		if (this == &other) return *this;

		*this->_contained_type = std::move(*other._contained_type.get());
		return *this;
	}

	bool ArrayType::operator==(const ArrayType& other) const noexcept
	{
		return *this->_contained_type == *other._contained_type;
	}

	auto ArrayType::operator<=>(const ArrayType& other) const noexcept
	{
		return std::tie(*this->_contained_type.get()) <=> std::tie(*other._contained_type.get());
	}

	ArrayType::operator std::string() const
	{
		std::stringstream ss;
		ss << std::string(*this->_contained_type);
		ss << "[]";
		return ss.str();
	}

	[[nodiscard]] const Type& ArrayType::type() const noexcept { return *_contained_type; }

	[[nodiscard]] Type& ArrayType::type() noexcept { return *_contained_type; }

	//
	// Struct
	//

	StructType::StructType(std::vector<Type> types) : _types(std::move(types)) {}

	auto StructType::operator<=>(const StructType& other) const noexcept
	{
		return std::lexicographical_compare_three_way(this->_types.begin(),
		                                              this->_types.end(),
		                                              other._types.begin(),
		                                              other._types.end(),
		                                              [](const auto& lhs, const auto& rhs) { return lhs <=> rhs; });
	}

	StructType::operator std::string() const
	{
		std::stringstream ss;
		ss << "(";
		for (size_t index = 0; index < this->_types.size(); ++index) {
			ss << std::string(this->_types[index]);
			if (index != this->_types.size() - 1) ss << ", ";
		}
		ss << ")";
		return ss.str();
	}

	[[nodiscard]] const std::vector<Type>& StructType::types() const noexcept { return this->_types; }

	[[nodiscard]] std::vector<Type>& StructType::types() noexcept { return this->_types; }

	//
	// Type
	//

	Type::operator std::string() const
	{
		return std::visit(
			[](const auto& arg) -> std::string {
				using T = std::decay_t<decltype(arg)>;
				if constexpr (std::is_same_v<T, ScalarType>) {
					static const std::unordered_map<ScalarType, std::string> k_types = {
						{ ScalarType::type_boolean,   "bool"    },
                        { ScalarType::type_character, "char"    },
						{ ScalarType::type_float,     "float"   },
                        { ScalarType::type_integer,   "integer" },
						{ ScalarType::type_string,    "str"     },
                        { ScalarType::type_void,      "void"    },
					};
					return k_types.at(arg);
				} else if constexpr (std::is_constructible_v<T, std::string>) {
					return std::string(arg);
				} else {
					return std::string("unknown type");
				}
			},
			_type);
	}

	std::strong_ordering operator<=>(const Type& lhs, const Type& rhs) noexcept
	{
		return std::tie(lhs._type) <=> std::tie(rhs._type);
	}
}  // namespace soul
