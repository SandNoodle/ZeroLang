#pragma once

#include "core/types.h"

namespace soul::backend::codegen::wasm
{
	/**
	 * @brief
	 * @see https://webassembly.github.io/spec/core/appendix/index-instructions.html
	 * @see https://pengowray.github.io/wasm-ops/
	 */
	enum class WASMOpcode : u8
	{
		Unreachable  = 0x00,
		Noop         = 0x01,
		Block        = 0x02,
		Loop         = 0x03,
		If           = 0x04,
		Else         = 0x05,
		End          = 0x0B,
		Return       = 0x0F,
		Call         = 0x10,
	};
}  // namespace soul::backend::codegen::wasm
