#include "backend/codegen/wasm/compiler.h"

#include "ast/nodes/assign.h"
#include "ast/nodes/binary.h"
#include "ast/nodes/cast.h"
#include "ast/nodes/for_loop.h"
#include "ast/nodes/foreach_loop.h"
#include "ast/nodes/function_declaration.h"
#include "ast/nodes/if.h"
#include "ast/nodes/literal.h"
#include "ast/nodes/module.h"
#include "ast/nodes/struct_declaration.h"
#include "ast/nodes/unary.h"
#include "ast/nodes/variable_declaration.h"

namespace soul::backend::codegen::wasm
{
	using namespace ast::nodes;
	using namespace ast::visitors;
	using namespace ast;

	namespace internal
	{
		/**
		 * @brief
		 * @see https://webassembly.github.io/spec/core/binary/modules.html#sections
		 */
		enum SectionType : u8
		{
			Custom    = 0,
			Type      = 1,
			Import    = 2,
			Function  = 3,
			Table     = 4,
			Memory    = 5,
			Global    = 6,
			Export    = 7,
			Start     = 8,
			Element   = 9,
			Code      = 10,
			Data      = 11,
			DataCount = 12,
		};

		/**
		 * @brief
		 * @see
		 */
		enum WASMTypeIndex : u8
		{
			I32        = 0x7F,
			I64        = 0x7E,
			F32        = 0x7D,
			F64        = 0x7C,
			V128       = 0x7B,
			FuncRef    = 0x70,
			ExternRef  = 0x6F,
			FuncType   = 0x60,
			ResultType = 0x40,
		};

		/**
		 * @brief
		 * @see
		 */
		struct WASMFunction
		{
			std::string name;
			// TODO
		};

		/**
		 * @brief
		 * @see
		 */
		struct WASMModule
		{
			std::string               name;
			std::vector<WASMFunction> functions;
			// TODO
		};
	}  // namespace internal

	WASMCompiler::WASMCompiler(ASTNode::Reference root) : _root(root) {}

	WASMCompiler::Bytecode WASMCompiler::compile(ASTNode::Reference root)
	{
		if (!dynamic_cast<ModuleNode*>(root)) {
			// TODO: Log error.
			return {};
		}
		return WASMCompiler{ root }.compile();
	}

	WASMCompiler::Bytecode WASMCompiler::compile() { return {}; }

	WASMCompiler::Bytecode WASMCompiler::serialize(const internal::WASMModule& module)
	{
		// Specification: https://webassembly.github.io/spec/core/binary/modules.html#binary-module
		WASMCompiler::Bytecode code;

		// 1. Write the magic number (\0asm)
		static constexpr std::array k_magic_number = { 0x00, 0x61, 0x73, 0x6D };
		code.insert(code.end(), k_magic_number.begin(), k_magic_number.end());

		// 2. Write the binary format version
		static constexpr std::array k_binary_version = { 0x01, 0x00, 0x00, 0x00 };
		code.insert(code.end(), k_binary_version.begin(), k_binary_version.end());

		// Note: Based on the specification, each following section is optional - omitted sections are equivalent to
		// being present with empty contents.
		// See: https://webassembly.github.io/spec/core/binary/modules.html#sections

		// 3. Custom section are intended for debugging information or third-party extensions, and are ignored by the
		// WebAssembly semantics. In our case, we use this section for placing compiler's metadata (such as version)
		// for debugging purposes.
		// See: https://webassembly.github.io/spec/core/binary/modules.html#custom-section
		static constexpr bool k_include_custom_metadata = true;  // FIXME: This should depend on a
		if constexpr (k_include_custom_metadata) {
			emit<u8>(code, internal::SectionType::Custom);
			Address section_size_address = emit<u32>(code, 0x00);
			// TODO
		}

		// 5. Type section contains all function signatures present in the module.
		// See: https://webassembly.github.io/spec/core/binary/modules.html#type-section
		{
			emit<u8>(code, internal::SectionType::Type);
			Address section_size_address = emit<u32>(code, 0x00);
			// TODO
		}

		// 6. Import section
		// See: https://webassembly.github.io/spec/core/binary/modules.html#import-section
		{
			emit<u8>(code, internal::SectionType::Import);
			Address section_size_address = emit<u32>(code, 0x00);
			// TODO
		}

		// 7. Function section
		// See: https://webassembly.github.io/spec/core/binary/modules.html#function-section
		{
			emit<u8>(code, internal::SectionType::Function);
			const Address section_size_address = emit<u32>(code, 0x00);
			emit<u32>(code, static_cast<u32>(module.functions.size()));
			for (const auto& function : module.functions) {
				emit<u8>(code, internal::WASMTypeIndex::FuncType);
				// TODO
				//	    emit_vector(code, std::span{ function.input_params });
				//	    emit_vector(code, std::span{ function.returns });
			}
			const Address current_address = static_cast<Address>(code.size());
			patch<u32>(code, section_size_address, static_cast<u32>(current_address - section_size_address));
		}

		// 8. Table section
		// See: https://webassembly.github.io/spec/core/binary/modules.html#table-section
		{
			emit<u8>(code, internal::SectionType::Table);
			Address section_size_address = emit<u32>(code, 0x00);
			// TODO
		}

		// 9. Memory section
		// See: https://webassembly.github.io/spec/core/binary/modules.html#memory-section
		{
			emit<u8>(code, internal::SectionType::Memory);
			Address section_size_address = emit<u32>(code, 0x00);
			// TODO
		}

		// 10. Global section
		// See: https://webassembly.github.io/spec/core/binary/modules.html#global-section
		{
			emit<u8>(code, internal::SectionType::Global);
			Address section_size_address = emit<u32>(code, 0x00);
			// TODO
		}

		// 11. Export section
		// See: https://webassembly.github.io/spec/core/binary/modules.html#export-section
		{
			emit<u8>(code, internal::SectionType::Export);
			Address section_size_address = emit<u32>(code, 0x00);
			// TODO
		}

		// 12. Start section
		// See: https://webassembly.github.io/spec/core/binary/modules.html#start-section
		{
			emit<u8>(code, internal::SectionType::Start);
			Address section_size_address = emit<u32>(code, 0x00);
			// TODO
		}

		// 13. Element section
		// See: https://webassembly.github.io/spec/core/binary/modules.html#element-section
		{
			emit<u8>(code, internal::SectionType::Element);
			Address section_size_address = emit<u32>(code, 0x00);
			// TODO
		}

		// 14. Code section
		// See: https://webassembly.github.io/spec/core/binary/modules.html#code-section
		{
			emit<u8>(code, internal::SectionType::Code);
			Address section_size_address = emit<u32>(code, 0x00);
			// TODO
		}

		// 15. Data section
		// See: https://webassembly.github.io/spec/core/binary/modules.html#data-section
		{
			emit<u8>(code, internal::SectionType::Data);
			Address section_size_address = emit<u32>(code, 0x00);
			// TODO
		}

		// 16. Data count section
		// See: https://webassembly.github.io/spec/core/binary/modules.html#data-count-section
		{
			emit<u8>(code, internal::SectionType::DataCount);
			Address section_size_address = emit<u32>(code, 0x00);
			// TODO
		}

		return code;
	}
}  // namespace soul::backend::codegen::wasm
