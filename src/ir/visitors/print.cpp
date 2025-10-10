#include "ir/visitors/print.h"

#include "ir/instruction.h"
#include "ir/ir.h"

#include <format>

namespace soul::ir::visitors
{

	using namespace std::string_view_literals;

	static constexpr auto k_nullptr = "__nullptr__"sv;

	std::string PrintVisitor::string() const { return _ss.str(); }

	void PrintVisitor::accept(const Module& module)
	{
		_ss << std::format("module @{};\n\n", module.name);
		for (const auto& function : module.functions) {
			if (!function) {
				_ss << std::format("@fn {}() :: {} {{}}", k_nullptr, std::string(types::Type{}));
				continue;
			}

			_ss << std::format("fn @{}", function->name);
			_ss << "(";
			for (std::size_t parameter_index = 0; parameter_index < function->parameters.size(); ++parameter_index) {
				_ss << std::string(function->parameters[parameter_index]);
				if (parameter_index != function->parameters.size() - 1) {
					_ss << ", ";
				}
			}
			_ss << ")";
			_ss << std::format(" :: {} {{\n", std::string(function->return_type));

			for (const auto& basic_block : function->basic_blocks) {
				if (!basic_block) {
					_ss << std::format("\t#{}:", k_nullptr);
					continue;
				}
				_ss << std::format("\t#{}:\n", basic_block->label());
				for (const auto& instruction : basic_block->instructions()) {
					if (!instruction) {
						_ss << k_nullptr;
						continue;
					}
					_ss << std::format("\t\t%{} = ", instruction->version);
#define SOUL_INSTRUCTION(name)          \
	if (instruction->is<name>()) {      \
		visit(instruction->as<name>()); \
	}
					SOUL_ALL_INSTRUCTIONS
#undef SOUL_INSTRUCTION
					_ss << std::format(" :: {}\n", std::string(instruction->type));
				}
				_ss << "\t\t; successors: [";
				for (std::size_t index = 0; index < basic_block->successors().size(); ++index) {
					_ss << std::format("#{}",
					                   basic_block->successors()[index]
					                       ? std::to_string(basic_block->successors()[index]->label())
					                       : k_nullptr);
					if (index != basic_block->successors().size() - 1) {
						_ss << ", ";
					}
				}
				_ss << "]\n";
			}
			_ss << "}\n";
		}
	}

	void PrintVisitor::visit(const Unreachable&) { _ss << "Unreachable()"sv; }

	void PrintVisitor::visit(const Noop&) { _ss << "Noop()"sv; }

	void PrintVisitor::visit(const Cast& instruction)
	{
		_ss << std::format("Cast(%{})", instruction.args[0] ? std::to_string(instruction.args[0]->version) : k_nullptr);
	}

	void PrintVisitor::visit(const Const& instruction)
	{
		_ss << std::format("Const(`{}`)", std::string(instruction.value));
	}

	void PrintVisitor::visit(const Jump& instruction)
	{
		_ss << std::format("Jump(#{})", instruction.target ? std::to_string(instruction.target->label()) : k_nullptr);
	}

	void PrintVisitor::visit(const JumpIf& instruction)
	{
		_ss << std::format("JumpIf(%{}, #{}, #{})",
		                   instruction.args[0] ? std::to_string(instruction.args[0]->version) : k_nullptr,
		                   instruction.then_block ? std::to_string(instruction.then_block->label()) : k_nullptr,
		                   instruction.else_block ? std::to_string(instruction.else_block->label()) : k_nullptr);
	}

	void PrintVisitor::visit([[maybe_unused]] const Phi& instruction) { _ss << "Phi()"sv; }

	void PrintVisitor::visit(const Upsilon& instruction)
	{
		_ss << std::format("Upsilon(%{}, ^%{})",
		                   instruction.args[0] ? std::to_string(instruction.args[0]->version) : k_nullptr,
		                   instruction.phi ? std::to_string(instruction.phi->version) : k_nullptr);
	}

	void PrintVisitor::visit(const Not& instruction)
	{
		_ss << std::format("Not(%{})", instruction.args[0] ? std::to_string(instruction.args[0]->version) : k_nullptr);
	}

#define SOUL_INSTRUCTION(name)                                                                              \
	void PrintVisitor::visit(const name& instruction)                                                       \
	{                                                                                                       \
		_ss << std::format("{}(%{}, %{})",                                                                  \
		                   #name,                                                                           \
		                   instruction.args[0] ? std::to_string(instruction.args[0]->version) : k_nullptr,  \
		                   instruction.args[1] ? std::to_string(instruction.args[1]->version) : k_nullptr); \
	}
	SOUL_ARITHMETIC_INSTRUCTIONS
	SOUL_COMPARISON_INSTRUCTIONS
	SOUL_LOGICAL_INSTRUCTIONS
#undef SOUL_INSTRUCTION

}  // namespace soul::ir::visitors
