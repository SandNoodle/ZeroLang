#include "ast/nodes/literal.h"

namespace soul
{
	literal_t::literal_t(const value_t& value)
		: _value(value) {}

	literal_t::dependency_t literal_t::create(const value_t& value)
	{
		return std::make_unique<literal_t>(value);
	}
}  // namespace soul
