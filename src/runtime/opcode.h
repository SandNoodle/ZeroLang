#pragma once

#include "core/types.h"

#include <string_view>

namespace soul
{
	enum class OpcodeType : u8
	{
		Noop,
	};

	/** * @brief Stringifies the opcode. */
	std::string_view opcode_to_string_view(OpcodeType type);
}  // namespace soul
