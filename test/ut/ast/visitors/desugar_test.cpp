#include "ast/visitors/desugar.h"

#include <gtest/gtest.h>

#include "ast/nodes/binary.h"
#include "ast/nodes/block.h"
#include "ast/nodes/for_loop.h"
#include "ast/nodes/literal.h"
#include "ast/nodes/module.h"
#include "ast/nodes/unary.h"
#include "ast/nodes/variable_declaration.h"
#include "ast/nodes/while.h"
#include "ast/visitors/stringify.h"

namespace soul::ast::visitors::ut
{
	using namespace types;
	using namespace ast::nodes;

	class DesugarVisitorTest : public ::testing::Test
	{
		public:
		static constexpr auto k_module_name = "desugar_module";
	};

	TEST_F(DesugarVisitorTest, ForLoop)
	{
		// Get this ForLoop node...
		auto for_loop_initialization = VariableDeclarationNode::create(
			"index", "i32", LiteralNode::create(Value{ 0 }, LiteralNode::Type::Int32), true);

		auto for_loop_condition
			= BinaryNode::create(LiteralNode::create(Value{ "index" }, LiteralNode::Type::Identifier),
		                         LiteralNode::create(Value{ 10 }, LiteralNode::Type::Int32),
		                         ASTNode::Operator::Less);

		auto for_loop_update = UnaryNode::create(LiteralNode::create(Value{ "index" }, LiteralNode::Type::Identifier),
		                                         ASTNode::Operator::Increment);

		auto for_loop_statements = ASTNode::Dependencies{};
		for_loop_statements.push_back(VariableDeclarationNode::create(
			"inner", "f32", LiteralNode::create(Value{ 3.14 }, LiteralNode::Type::Float32), false));

		auto for_loop = ForLoopNode::create(std::move(for_loop_initialization),
		                                    std::move(for_loop_condition),
		                                    std::move(for_loop_update),
		                                    BlockNode::create(std::move(for_loop_statements)));

		auto input_module_statements = ASTNode::Dependencies{};
		input_module_statements.push_back(std::move(for_loop));
		auto input_module = ModuleNode::create(k_module_name, std::move(input_module_statements));

		// ...desugar it...
		DesugarVisitor desugar_visitor{};
		desugar_visitor.accept(input_module.get());

		auto result_module = desugar_visitor.cloned();

		// ...and verify the results.
		auto while_node_initialization = VariableDeclarationNode::create(
			"index", "i32", LiteralNode::create(Value{ 0 }, LiteralNode::Type::Int32), true);

		auto while_node_condition
			= BinaryNode::create(LiteralNode::create(Value{ "index" }, LiteralNode::Type::Identifier),
		                         LiteralNode::create(Value{ 10 }, LiteralNode::Type::Int32),
		                         ASTNode::Operator::Less);

		auto while_node_statements = ASTNode::Dependencies{};
		while_node_statements.reserve(2);
		while_node_statements.push_back(VariableDeclarationNode::create(
			"inner", "f32", LiteralNode::create(Value{ 3.14 }, LiteralNode::Type::Float32), false));
		while_node_statements.push_back(UnaryNode::create(
			LiteralNode::create(Value{ "index" }, LiteralNode::Type::Identifier), ASTNode::Operator::Increment));

		auto while_node
			= WhileNode::create(std::move(while_node_condition), BlockNode::create(std::move(while_node_statements)));

		auto outer_statements = ASTNode::Dependencies{};
		outer_statements.reserve(2);
		outer_statements.push_back(std::move(while_node_initialization));
		outer_statements.push_back(std::move(while_node));

		auto expected_module_statements = ASTNode::Dependencies{};
		expected_module_statements.push_back(BlockNode::create(std::move(outer_statements)));
		auto expected_module = ModuleNode::create(k_module_name, std::move(expected_module_statements));

		StringifyVisitor stringify_result{};
		stringify_result.accept(result_module.get());
		StringifyVisitor stringify_expected{};
		stringify_expected.accept(expected_module.get());

		ASSERT_EQ(stringify_expected.string(), stringify_result.string());
	}
}  // namespace soul::ast::visitors::ut
