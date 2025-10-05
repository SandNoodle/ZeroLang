#pragma once

#include "ir/basic_block.h"
#include "ir/instruction.h"
#include "ir/ir.h"

#include <memory>
#include <ranges>
#include <vector>

namespace soul::ir
{
	/**
	 * @brief IRBuilder is a helper class, which simplifies and enables building of IR programs.
	 */
	class IRBuilder
	{
		private:
		std::unique_ptr<Module> _module{};
		BasicBlock*             _current_block{};
		BasicBlock::Label       _next_block_version{ 0 };
		Instruction::Version    _next_instruction_version{ 0 };

		public:
		constexpr IRBuilder();
		constexpr IRBuilder(const IRBuilder&)     = delete;
		constexpr IRBuilder(IRBuilder&&) noexcept = default;
		constexpr ~IRBuilder()                    = default;

		constexpr IRBuilder& operator=(const IRBuilder&)     = delete;
		constexpr IRBuilder& operator=(IRBuilder&&) noexcept = default;

		constexpr std::unique_ptr<Module> build() noexcept;

		constexpr void set_module_name(std::string_view name);

		/**
		 * @brief Creates a new function in the module (with a single basic block initialized).
		 * @warning Switches the current basic block to a newly initialized one.
		 */
		constexpr void create_function(std::string_view         identifier,
		                               types::Type              return_type,
		                               std::vector<types::Type> parameters);

		constexpr void        switch_to(BasicBlock* block);
		constexpr BasicBlock* create_basic_block();
		constexpr BasicBlock* current_basic_block() const noexcept { return _current_block; }

		constexpr void connect(const std::ranges::forward_range auto& predecessors, BasicBlock* successor);
		constexpr void connect(BasicBlock* predecessor, const std::ranges::forward_range auto& successors);
		constexpr void connect(BasicBlock* predecessor, BasicBlock* successor);

		/**
		 * @brief Constructs new Instruction and appends it to the end of the current BasicBlock.
		 * @tparam Inst Instruction to be emitted.
		 * @tparam Args Arguments used to construct the Instruction.
		 * @return Pointer to the instruction emitted.
		 */
		template <InstructionKind Inst, typename... Args>
		constexpr Instruction* emit(Args&&... args);
	};
}  // namespace soul::ir
#include "ir/builder.inl"
