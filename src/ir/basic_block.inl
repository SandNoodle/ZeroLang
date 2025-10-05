#pragma once
namespace soul::ir
{
	constexpr BasicBlock::BasicBlock(Label label) : _label(label) {}

	constexpr auto BasicBlock::label() const noexcept -> Label { return _label; }

	constexpr auto BasicBlock::instructions() const noexcept -> const Instructions& { return _instructions; }

	constexpr auto BasicBlock::successors() const noexcept -> const BasicBlocks& { return _successors; }
}  // namespace soul::ir
