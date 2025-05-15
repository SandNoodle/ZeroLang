#pragma once

#include "ast/ast.h"
#include "ast/visitors/default_traverse.h"
#include "backend/wasm/opcode.h"
#include "core/types.h"

#include <span>
#include <vector>

namespace soul::backend::wasm
{
	namespace internal
	{
		struct WASMModule;
		struct WASMFunction;
	}

	/**
	 * @brief
	 */
	class WASMCompiler final
	{
		public:
		using Byte     = u8;
		using Bytecode = std::vector<Byte>;

		private:
		using Address = std::size_t;

		private:
		ast::ASTNode::Reference _root = nullptr;

		public:
		/**
		 * @brief
		 * @param[in] root
		 * @return
		 */
		[[nodiscard]] static Bytecode compile(ast::ASTNode::Reference root);

		private:
		WASMCompiler(ast::ASTNode::Reference root);

		// Compilation.
		Bytecode compile();

		// Serialization.
		Bytecode serialize(const internal::WASMModule& module);

		template <typename T>
		constexpr Address emit(Bytecode& code, T v);

		template <WASMOpcode T>
		constexpr Address emit(Bytecode& code);

		template <typename T>
		constexpr void emit_vector(Bytecode& code, std::span<const T> values);

		constexpr void emit_string(Bytecode& code, std::string_view string);

		template <typename T>
		constexpr void patch(Bytecode& code, Address address, T value);
	};
}  // namespace soul::backend::wasm
#include "backend/wasm/compiler.inl"
