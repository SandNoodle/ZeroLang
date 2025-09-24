#pragma once

#include "common/types/type.h"
#include "common/value.h"
#include "core/types.h"

#include <array>
#include <type_traits>

namespace soul::ir
{
	class BasicBlock;
	struct Instruction;

	/**
	 * @brief InstructionKind is a concept, which holds true for any class that inherits from an Instruction class.
	 */
	template <typename T>
	concept InstructionKind = std::derived_from<T, Instruction>;

	/**
	 * @brief Represents a single Instruction in the language's Intermediate Representation.
	 * @detials Represented in Static Single-Assignment (SSA) Three-Address Code (TAC) form.
	 */
	struct [[gnu::packed]] Instruction
	{
		public:
		using Version   = u32;
		using Arguments = std::array<Instruction*, 2>;

		static constexpr Version k_invalid_version = std::numeric_limits<Version>::max();

		public:
		Version     version{ k_invalid_version };
		types::Type type{};
		Arguments   args{ Instruction::no_args() };

		public:
		constexpr Instruction(types::Type type, Arguments args);

		virtual ~Instruction() = default;

		constexpr bool operator==(const Instruction& other) const noexcept  = default;
		constexpr auto operator<=>(const Instruction& other) const noexcept = default;

		/** @brief Verifies if an Instruction is of a given type. */
		template <InstructionKind T>
		[[nodiscard]] constexpr bool is() const noexcept;

		/**
		 * @brief Returns the requested underlying type.
		 * @important Does not perform any validation - assumes that Instruction::is<T> was used first.
		 */
		template <InstructionKind T>
		[[nodiscard]] constexpr const T& as() const noexcept;

		/**
		 * @brief Returns the requested underlying type.
		 * @important Does not perform any validation - assumes that Instruction::is<T> was used first.
		 */
		template <InstructionKind T>
		[[nodiscard]] constexpr T& as() noexcept;

		protected:
		[[nodiscard]] static constexpr Arguments no_args() noexcept;
		[[nodiscard]] static constexpr Arguments single_arg(Instruction*) noexcept;
		[[nodiscard]] static constexpr Arguments two_args(Instruction*, Instruction*) noexcept;
	};

	/**
	 * @brief Unreachable instruction: causes an immediate trap signal; signals that the program is in an invalid state.
	 */
	struct Unreachable final : public Instruction
	{
		public:
		constexpr Unreachable();
		virtual ~Unreachable() override = default;

		constexpr bool operator==(const Unreachable& other) const noexcept  = default;
		auto           operator<=>(const Unreachable& other) const noexcept = default;
	};

	/**
	 * @brief Noop instruction: no operation; does nothing.
	 */
	struct Noop final : public Instruction
	{
		public:
		constexpr Noop();
		virtual ~Noop() override = default;

		constexpr bool operator==(const Noop& other) const noexcept  = default;
		auto           operator<=>(const Noop& other) const noexcept = default;
	};

	/**
	 * @brief Cast instruction: performs type conversion of a given expression.
	 */
	struct Cast final : public Instruction
	{
		public:
		constexpr Cast(types::Type type, Instruction* arg);
		virtual ~Cast() override = default;

		constexpr bool operator==(const Cast& other) const noexcept  = default;
		auto           operator<=>(const Cast& other) const noexcept = default;
	};

	/**
	 * @brief Const instruction: holds an (immediate) value of a given type.
	 */
	struct Const final : public Instruction
	{
		public:
		Value value;

		public:
		constexpr Const(types::Type type, Value value);
		virtual ~Const() override = default;

		constexpr bool operator==(const Const& other) const noexcept  = default;
		auto           operator<=>(const Const& other) const noexcept = default;
	};

	/**
	 * @brief Jump instruction: performs an unconditional jump to a given target (BasicBlock).
	 */
	struct Jump final : public Instruction
	{
		public:
		BasicBlock* target;

		public:
		constexpr Jump(BasicBlock* target);
		virtual ~Jump() override = default;

		constexpr bool operator==(const Jump& other) const noexcept  = default;
		auto           operator<=>(const Jump& other) const noexcept = default;
	};

	/**
	 * @brief JumpIf instruction: performs a conditional jump to either of two targets (BasicBlocks).
	 */
	struct JumpIf final : public Instruction
	{
		public:
		BasicBlock* then_block;
		BasicBlock* else_block;

		public:
		// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
		constexpr JumpIf(Instruction* condition, BasicBlock* then_block, BasicBlock* else_block);

		constexpr bool operator==(const JumpIf& other) const noexcept  = default;
		auto           operator<=>(const JumpIf& other) const noexcept = default;
	};

	/**
	 * @brief Not instruction: performs a negation of an input (boolean) expression.
	 */
	struct Not final : public Instruction
	{
		public:
		constexpr Not(Instruction* arg);
		virtual ~Not() override = default;

		constexpr bool operator==(const Not& other) const noexcept  = default;
		auto           operator<=>(const Not& other) const noexcept = default;
	};

	/**
	 * @brief Arithmetic instructions: performs an arithmetic operation on two expressions.
	 */
#define SOUL_ARITHMETIC_INSTRUCTIONS \
	SOUL_INSTRUCTION(Add)            \
	SOUL_INSTRUCTION(Sub)            \
	SOUL_INSTRUCTION(Mul)            \
	SOUL_INSTRUCTION(Div)            \
	SOUL_INSTRUCTION(Mod)

	/**
	 * @brief Comparison instructions: compare two expressions; returns boolean result.
	 */
#define SOUL_COMPARISON_INSTRUCTIONS \
	SOUL_INSTRUCTION(Equal)          \
	SOUL_INSTRUCTION(NotEqual)       \
	SOUL_INSTRUCTION(Greater)        \
	SOUL_INSTRUCTION(GreaterEqual)   \
	SOUL_INSTRUCTION(Less)           \
	SOUL_INSTRUCTION(LessEqual)

	/**
	 * @brief Logical instructions: perform boolean arithmetic on two expressions; returns boolean result.
	 */
#define SOUL_LOGICAL_INSTRUCTIONS \
	SOUL_INSTRUCTION(And)         \
	SOUL_INSTRUCTION(Or)

#define SOUL_INSTRUCTION(name)                                                  \
	struct name final : public Instruction                                      \
	{                                                                           \
		public:                                                                 \
		constexpr name(types::Type type, Instruction* arg0, Instruction* arg1)  \
			: Instruction(std::move(type), Instruction::two_args(arg0, arg1))   \
		{                                                                       \
		}                                                                       \
		virtual ~name() override                                     = default; \
		constexpr bool operator==(const name& other) const noexcept  = default; \
		auto           operator<=>(const name& other) const noexcept = default; \
	};
	SOUL_ARITHMETIC_INSTRUCTIONS
#undef SOUL_INSTRUCTION

#define SOUL_INSTRUCTION(name)                                                                                   \
	struct name final : public Instruction                                                                       \
	{                                                                                                            \
		public:                                                                                                  \
		constexpr name(Instruction* arg0, Instruction* arg1)                                                     \
			: Instruction(types::Type{ types::PrimitiveType::Kind::Boolean }, Instruction::two_args(arg0, arg1)) \
		{                                                                                                        \
		}                                                                                                        \
		virtual ~name() override                                     = default;                                  \
		constexpr bool operator==(const name& other) const noexcept  = default;                                  \
		auto           operator<=>(const name& other) const noexcept = default;                                  \
	};
	SOUL_COMPARISON_INSTRUCTIONS
	SOUL_LOGICAL_INSTRUCTIONS
#undef SOUL_INSTRUCTION

#undef SOUL_ARITHMETIC_INSTRUCTIONS
#undef SOUL_COMPARISON_INSTRUCTIONS
#undef SOUL_LOGICAL_INSTRUCTIONS
}  // namespace soul::ir
#include "ir/instruction.inl"
