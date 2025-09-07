#include "ast/visitors/copy.h"

#include <gtest/gtest.h>

#include "ast/nodes/binary.h"
#include "ast/nodes/block.h"
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
#include "ast/visitors/stringify.h"

#include <string>

namespace soul::ast::visitors
{
	using namespace ast;
	using namespace ast::nodes;

	class CopyVisitorTest : public ::testing::Test
	{
	};

	TEST_F(CopyVisitorTest, All)
	{
		auto struct_declaration_members = ASTNode::Dependencies{};
		struct_declaration_members.reserve(3);
		struct_declaration_members.emplace_back(VariableDeclarationNode::create("my_int", "i32", nullptr, false));
		struct_declaration_members.emplace_back(VariableDeclarationNode::create("my_str", "str", nullptr, false));
		struct_declaration_members.emplace_back(VariableDeclarationNode::create("my_float", "f64", nullptr, false));
		auto struct_declaration = StructDeclarationNode::create("my_struct", std::move(struct_declaration_members));

		auto if_node_true_statements = ASTNode::Dependencies{};
		if_node_true_statements.emplace_back(
			UnaryNode::create(LiteralNode::create(Value{ "index" }), ASTNodeOperator::Decrement));
		auto if_node_false_statements = ASTNode::Dependencies{};
		if_node_false_statements.emplace_back(
			UnaryNode::create(LiteralNode::create(Value{ "index" }), ASTNodeOperator::Increment));

		auto if_node = IfNode::create(LiteralNode::create(Value{ true }),
		                              BlockNode::create(std::move(if_node_true_statements)),
		                              BlockNode::create(std::move(if_node_false_statements)));

		auto for_loop_initialization
			= VariableDeclarationNode::create("index", "i32", LiteralNode::create(Value{ 0 }), false);
		auto for_loop_condition = BinaryNode::create(
			LiteralNode::create(Value{ "index" }), LiteralNode::create(Value{ 10 }), ASTNodeOperator::LessEqual);
		auto for_loop_update     = UnaryNode::create(LiteralNode::create(Value{ "index" }), ASTNodeOperator::Increment);
		auto for_loop_statements = ASTNode::Dependencies{};
		for_loop_statements.push_back(std::move(if_node));

		auto for_loop = ForLoopNode::create(std::move(for_loop_initialization),
		                                    std::move(for_loop_condition),
		                                    std::move(for_loop_update),
		                                    BlockNode::create(std::move(for_loop_statements)));

		auto function_declaration_statements = ASTNode::Dependencies{};
		function_declaration_statements.push_back(std::move(for_loop));
		auto function_declaration_parameters = ASTNode::Dependencies{};
		function_declaration_parameters.reserve(2);
		function_declaration_parameters.emplace_back(VariableDeclarationNode::create("a", "i32", nullptr, false));
		function_declaration_parameters.emplace_back(VariableDeclarationNode::create("b", "chr", nullptr, false));
		auto function_declaration
			= FunctionDeclarationNode::create("my_function",
		                                      "my_struct",
		                                      std::move(function_declaration_parameters),
		                                      BlockNode::create(std::move(function_declaration_statements)));

		auto module_statements = ASTNode::Dependencies{};
		module_statements.reserve(2);
		module_statements.push_back(std::move(struct_declaration));
		module_statements.push_back(std::move(function_declaration));
		auto expected_module = ModuleNode::create("copy_module", std::move(module_statements));

		StringifyVisitor stringify_expected_before{};
		stringify_expected_before.accept(expected_module.get());

		CopyVisitor copy_visitor{};
		copy_visitor.accept(expected_module.get());

		StringifyVisitor stringify_expected_after{};
		stringify_expected_after.accept(expected_module.get());

		EXPECT_EQ(stringify_expected_before.string(), stringify_expected_after.string());

		const auto& result_module = copy_visitor.cloned();

		StringifyVisitor stringify_result{};
		stringify_result.accept(result_module.get());

		EXPECT_NE(expected_module.get(), result_module.get());
		ASSERT_EQ(stringify_result.string(), stringify_expected_before.string());
	}
}  // namespace soul::ast::visitors
