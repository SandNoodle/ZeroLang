#include "common/value.h"

namespace soul
{
	Value::Value(Variant value) : value(std::move(value)) {}

	std::partial_ordering Value::operator<=>(const Value& other) const noexcept
	{
		return std::tie(value) <=> std::tie(other.value);
	}

	Value::operator std::string() const
	{
		return std::visit(
			[](const auto& v) -> std::string {
				if constexpr (std::is_same_v<std::remove_cvref_t<decltype(v)>, std::monostate>) {
					return std::string("__unknown__");
				} else if constexpr (std::is_constructible_v<std::decay_t<decltype(v)>, std::string>) {
					return std::string(v);
				} else {
					std::stringstream ss;
					ss << std::boolalpha << v;
					return ss.str();
				}
			},
			value);
	}

}  // namespace soul
