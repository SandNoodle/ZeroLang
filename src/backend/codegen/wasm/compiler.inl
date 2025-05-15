#pragma once

#include <cstring>
#include <type_traits>

namespace soul::backend::codegen::wasm
{
	template <typename T>
	constexpr auto WASMCompiler::emit(Bytecode& code, T value) -> Address
	{
		return 0;
	}

	template <WASMOpcode T>
	constexpr auto WASMCompiler::emit(Bytecode& code) -> Address
	{
		return 0;
	}

	template <typename T>
	constexpr auto WASMCompiler::emit_vector(Bytecode& code, std::span<const T> values) -> void
	{
	}

	constexpr auto WASMCompiler::emit_string(Bytecode& code, std::string_view string) -> void
	{
		emit<u32>(code, static_cast<u32>(string.size()));
		// TODO: Copy string to a buffer.
	}

	template <typename T>
	constexpr auto WASMCompiler::patch(Bytecode& code, Address address, T value) -> void
	{
	}
}  // namespace soul::backend::codegen::wasm
