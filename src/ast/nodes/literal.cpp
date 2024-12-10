#include "ast/nodes/literal.h"

#include <sstream>
#include <utility>

namespace soul
{
	LiteralNode::LiteralNode(ValueType value) : value(std::move(value)) {}

	LiteralNode::Dependency LiteralNode::create(ValueType value)
	{
		return std::make_unique<LiteralNode>(std::move(value));
	}

	LiteralNode::operator std::string() const noexcept
	{
		return std::visit(
			[](const auto& v) -> std::string {
				if constexpr (std::is_same_v<std::remove_cvref_t<decltype(v)>, std::monostate>) {
					return "__unknown__";
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
