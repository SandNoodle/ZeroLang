#pragma once

#include <cassert>

namespace soul::ir
{
	constexpr IRBuilder::IRBuilder() : _module(std::make_unique<Module>("")) {}

	constexpr auto IRBuilder::build() noexcept -> std::unique_ptr<Module> { return std::move(_module); }

	constexpr auto IRBuilder::set_module_name(std::string_view name) -> void { _module->name = std::string(name); }

	constexpr auto IRBuilder::create_function(std::string_view         identifier,
	                                          types::Type              return_type,
	                                          std::vector<types::Type> parameters) -> void
	{
		_module->functions.emplace_back(
			std::make_unique<Function>(identifier, std::move(return_type), std::move(parameters)));
		_current_block = create_basic_block();
	}

	constexpr auto IRBuilder::switch_to(BasicBlock* block) -> void
	{
		assert(block && "uninitialized block was passed (nullptr)");
		_current_block = block;
	}

	constexpr auto IRBuilder::create_basic_block() -> BasicBlock*
	{
		auto& current_function = _module->functions.back();
		current_function->basic_blocks.emplace_back(std::make_unique<BasicBlock>(_next_block_version++));
		return current_function->basic_blocks.back().get();
	}

	constexpr auto IRBuilder::connect(const std::ranges::forward_range auto& predecessors, BasicBlock* successor)
		-> void
	{
		for (const auto& predecessor : predecessors) {
			connect(predecessor, successor);
		}
	}

	constexpr auto IRBuilder::connect(BasicBlock* predecessor, const std::ranges::forward_range auto& successors)
		-> void
	{
		assert(predecessor && "invalid predecessor (BasicBlock) was passed (nullptr)");
		predecessor->_successors.reserve(predecessor->_successors.size() + successors.size());
		for (const auto& successor : successors) {
			connect(predecessor, successor);
		}
	}

	constexpr auto IRBuilder::connect(BasicBlock* predecessor, BasicBlock* successor) -> void
	{
		assert(predecessor && "invalid predecessor (BasicBlock) was passed (nullptr)");
		assert(successor && "invalid successor (BasicBlock) was passed (nullptr)");
		predecessor->_successors.emplace_back(successor);
	}

	template <InstructionKind Inst, typename... Args>
	constexpr auto IRBuilder::emit(Args&&... args) -> Instruction*
	{
		assert(_current_block && "_current_block was not initialized properly (nullptr)");
		assert(_current_block->_label != BasicBlock::k_invalid_label && "_current_block is invalid (k_invalid_label)");
		_current_block->_instructions.emplace_back(std::make_unique<Inst>(std::forward<Args>(args)...));
		_current_block->_instructions.back()->version = _next_instruction_version++;
		return static_cast<Inst*>(_current_block->_instructions.back().get());
	}
}  // namespace soul::ir
