namespace soul::ir
{
	template <InstructionKind T>
	constexpr auto Instruction::is() const noexcept -> bool
	{
		return dynamic_cast<const T*>(this) != nullptr;
	}

	template <InstructionKind T>
	constexpr auto Instruction::as() const noexcept -> const T&
	{
		return dynamic_cast<const T&>(*this);
	}

	template <InstructionKind T>
	constexpr auto Instruction::as() noexcept -> T&
	{
		return dynamic_cast<T&>(*this);
	}

	constexpr auto Instruction::no_args() noexcept -> Arguments { return Arguments{ nullptr, nullptr }; }

	constexpr auto Instruction::single_arg(Instruction* arg) noexcept -> Arguments { return Arguments{ arg, nullptr }; }

	constexpr auto Instruction::two_args(Instruction* arg0, Instruction* arg1) noexcept -> Arguments
	{
		return Arguments{ arg0, arg1 };
	}

	constexpr Instruction::Instruction(types::Type type, Arguments args) : type(std::move(type)), args(std::move(args))
	{
	}

	constexpr Unreachable::Unreachable() : Instruction(types::Type{}, Instruction::no_args()) {}

	constexpr Noop::Noop() : Instruction(types::Type{ types::PrimitiveType::Kind::Void }, Instruction::no_args()) {}

	constexpr Cast::Cast(types::Type type, Instruction* arg)
		: Instruction(std::move(type), Instruction::single_arg(arg))
	{
	}

	constexpr Const::Const(types::Type type, Value value)
		: Instruction(std::move(type), Instruction::no_args()), value(std::move(value))
	{
	}

	constexpr Jump::Jump(BasicBlock* target)
		: Instruction(types::Type{ types::PrimitiveType::Kind::Void }, Instruction::no_args()), target(target)
	{
	}

	constexpr JumpIf::JumpIf(Instruction* condition, BasicBlock* then_block, BasicBlock* else_block)
		: Instruction(types::PrimitiveType::Kind::Void, Instruction::single_arg(condition)),
		  then_block(then_block),
		  else_block(else_block)
	{
	}

	constexpr Not::Not(Instruction* arg)
		: Instruction(types::Type{ types::PrimitiveType::Kind::Boolean }, Instruction::single_arg(arg))
	{
	}
};  // namespace soul::ir
