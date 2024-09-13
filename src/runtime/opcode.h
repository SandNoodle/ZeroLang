#pragma once

#include "common/types.h"

#include <string_view>

namespace soul
{
	enum class opcode_type_t : u8
	{
		// clang-format-off
		noop,

		// Comparison
		opcode_equal, opcode_not_equal,
		opcode_less, opcode_less_equal,
		opcode_greater, opcode_greater_equal,

		// Arithmetic
		opcode_add_integer, opcode_add_float,
		opcode_sub_integer, opcode_sub_float,
		opcode_mul_integer, opcode_mul_float,
		opcode_div_integer, opcode_div_float,
		opcode_modulo,
		opcode_bitwise_shift_left, opcode_bitwise_shift_right,
		opcode_bitwise_xor, opcode_bitwise_or, opcode_bitwise_and,

		// Memory
		opcode_load_variable,
		opcode_store_variable,
		opcode_load_constant_integer,
		opcode_load_constant_float,
		opcode_load_constant_string,

		// Control flow
		opcode_jump,
		opcode_jump_equal, opcode_jump_not_equal,
		opcode_jump_less, opcode_jump_less_equal,
		opcode_jump_greater, opcode_jump_greater_equal,
		opcode_return,

		// clang-format on
	};

	/** * @brief Stringifies the opcode. */
	std::string_view opcode_to_string_view(opcode_type_t type);
}  // namespace soul
