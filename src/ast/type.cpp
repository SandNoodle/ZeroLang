#include "ast/type.h"

#include <unordered_map>
#include <sstream>

namespace soul
{
	//
	// Array
	//

	array_type_t::array_type_t(const array_type_t& other) noexcept
		: _contained_type(std::make_unique<type_t>(*other._contained_type.get())) {}

	array_type_t::array_type_t(array_type_t&& other) noexcept
		: _contained_type(std::make_unique<type_t>(*other._contained_type.get())) {}

	array_type_t::array_type_t(type_t contained_type)
		: _contained_type(std::make_unique<type_t>(std::move(contained_type))) {}

	array_type_t& array_type_t::operator=(const array_type_t& other) noexcept
	{
		if (this == &other)
			return *this;

		this->_contained_type = std::make_unique<type_t>(*other._contained_type.get());
		return *this;
	}

	array_type_t& array_type_t::operator=(array_type_t&& other) noexcept
	{
		if (this == &other)
			return *this;

		*this->_contained_type = std::move(*other._contained_type.get());
		return *this;
	}

	bool array_type_t::operator==(const array_type_t& other) const noexcept
	{
		return *this->_contained_type == *other._contained_type;
	}

	auto array_type_t::operator<=>(const array_type_t& other) const noexcept
	{
		return std::tie(*this->_contained_type.get()) <=> std::tie(*other._contained_type.get());
	}

	array_type_t::operator std::string() const
	{
		std::stringstream ss;
		ss << std::string(*this->_contained_type);
		ss << "[]";
		return ss.str();
	}

	[[nodiscard]] const type_t& array_type_t::type() const noexcept
	{
		return *_contained_type;
	}

	[[nodiscard]] type_t& array_type_t::type() noexcept
	{
		return *_contained_type;
	}

	//
	// Struct
	//

	struct_type_t::struct_type_t(std::vector<type_t> types)
		: _types(std::move(types)) {}

	auto struct_type_t::operator<=>(const struct_type_t& other) const noexcept
	{
		return std::lexicographical_compare_three_way(this->_types.begin(),
			                                          this->_types.end(),
			                                          other._types.begin(),
			                                          other._types.end(),
			                                          [](const auto& lhs, const auto& rhs) {
			                                          	return lhs <=> rhs;
			                                          });
	}

	struct_type_t::operator std::string() const
	{
		std::stringstream ss;
		ss << "(";
		for (size_t index = 0; index < this->_types.size(); ++index)
		{
			ss << std::string(this->_types[index]);
			if (index != this->_types.size() - 1)
				ss << ", ";
		}
		ss << ")";
		return ss.str();
	}

    [[nodiscard]] const std::vector<type_t>& struct_type_t::types() const noexcept
	{
		return this->_types;
	}

	[[nodiscard]] std::vector<type_t>& struct_type_t::types() noexcept
	{
		return this->_types;
	}

	//
	// Type
	//

	type_t::operator std::string() const
	{
		return std::visit([](const auto& arg) -> std::string {
			using T = std::decay_t<decltype(arg)>;
			if constexpr (std::is_same_v<T, scalar_type_t>) {
				static const std::unordered_map<scalar_type_t, std::string> k_types = {
					{ scalar_type_t::type_boolean,   "bool" },
					{ scalar_type_t::type_character, "char" },
					{ scalar_type_t::type_float,     "float" },
					{ scalar_type_t::type_integer,   "integer" },
					{ scalar_type_t::type_string,    "str" },
					{ scalar_type_t::type_void,      "void" },
				};
				return k_types.at(arg);
			} else if constexpr (std::is_constructible_v<T, std::string>) {
				return std::string(arg);
			} else {
				return std::string("unknown type");
			}
		}, _type);
	}

	std::strong_ordering operator<=>(const type_t& lhs, const type_t& rhs) noexcept
	{
		return std::tie(lhs._type) <=> std::tie(rhs._type);
	}
}  // namespace soul
