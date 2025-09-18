#include "ast/visitors/error_collector.h"

#include <gtest/gtest.h>

#include "ast/nodes/binary.h"
#include "ast/nodes/block.h"
#include "ast/nodes/cast.h"
#include "ast/nodes/error.h"
#include "ast/nodes/for_loop.h"
#include "ast/nodes/foreach_loop.h"
#include "ast/nodes/function_declaration.h"
#include "ast/nodes/if.h"
#include "ast/nodes/literal.h"
#include "ast/nodes/module.h"
#include "ast/nodes/struct_declaration.h"
#include "ast/nodes/unary.h"
#include "ast/nodes/variable_declaration.h"

namespace soul::ast::visitors::ut
{
	using namespace ast::nodes;

	class ErrorCollectorTest : public ::testing::Test
	{
	};

	TEST_F(ErrorCollectorTest, NothingToCollect)
	{
		auto struct_declaration_members = ASTNode::Dependencies{};
		struct_declaration_members.reserve(3);
		struct_declaration_members.emplace_back(VariableDeclarationNode::create("my_int", "i32", nullptr, false));
		struct_declaration_members.emplace_back(VariableDeclarationNode::create("my_str", "str", nullptr, false));
		struct_declaration_members.emplace_back(VariableDeclarationNode::create("my_float", "f64", nullptr, false));
		auto struct_declaration = StructDeclarationNode::create("my_struct", std::move(struct_declaration_members));

		auto if_node_true_statements = ASTNode::Dependencies{};
		if_node_true_statements.emplace_back(
			UnaryNode::create(LiteralNode::create(Value{ "index" }), ASTNode::Operator::Decrement));
		auto if_node_false_statements = ASTNode::Dependencies{};
		if_node_false_statements.emplace_back(
			UnaryNode::create(LiteralNode::create(Value{ "index" }), ASTNode::Operator::Increment));

		auto if_node = IfNode::create(LiteralNode::create(Value{ true }),
		                              BlockNode::create(std::move(if_node_true_statements)),
		                              BlockNode::create(std::move(if_node_false_statements)));

		auto for_loop_initialization
			= VariableDeclarationNode::create("index", "i32", LiteralNode::create(Value{ 0 }), false);
		auto for_loop_condition = BinaryNode::create(
			LiteralNode::create(Value{ "index" }), LiteralNode::create(Value{ 10 }), ASTNode::Operator::LessEqual);
		auto for_loop_update = UnaryNode::create(LiteralNode::create(Value{ "index" }), ASTNode::Operator::Increment);
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
		auto module = ModuleNode::create("error_collector_module", std::move(module_statements));

		ErrorCollectorVisitor error_collector{};
		error_collector.accept(module.get());

		EXPECT_TRUE(error_collector.is_valid());
	}

	TEST_F(ErrorCollectorTest, ErrorsUpToDepth)
	{
		auto struct_declaration_members = ASTNode::Dependencies{};
		struct_declaration_members.reserve(3);
		struct_declaration_members.emplace_back(VariableDeclarationNode::create("my_int", "i32", nullptr, false));
		struct_declaration_members.emplace_back(VariableDeclarationNode::create("my_str", "str", nullptr, false));
		struct_declaration_members.emplace_back(ErrorNode::create("struct_declaration_error"));
		auto struct_declaration = StructDeclarationNode::create("my_struct", std::move(struct_declaration_members));

		auto if_node_true_statements = ASTNode::Dependencies{};
		if_node_true_statements.emplace_back(
			UnaryNode::create(LiteralNode::create(Value{ "index" }), ASTNode::Operator::Decrement));
		auto if_node_false_statements = ASTNode::Dependencies{};
		if_node_false_statements.emplace_back(
			UnaryNode::create(ErrorNode::create("if_false_unary_expr_error"), ASTNode::Operator::Increment));

		auto if_node = IfNode::create(LiteralNode::create(Value{ true }),
		                              BlockNode::create(std::move(if_node_true_statements)),
		                              BlockNode::create(std::move(if_node_false_statements)));

		auto for_loop_initialization
			= VariableDeclarationNode::create("index", "i32", LiteralNode::create(Value{ 0 }), false);
		auto for_loop_condition = BinaryNode::create(LiteralNode::create(Value{ "index" }),
		                                             ErrorNode::create("for_loop_condition_binary_rhs_expr_error"),
		                                             ASTNode::Operator::LessEqual);
		auto for_loop_update = UnaryNode::create(LiteralNode::create(Value{ "index" }), ASTNode::Operator::Increment);
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
		module_statements.push_back(ErrorNode::create("module_error"));
		module_statements.push_back(std::move(function_declaration));
		auto module = ModuleNode::create("error_collector_module", std::move(module_statements));

		ErrorCollectorVisitor error_collector{ 3 };
		error_collector.accept(module.get());

		EXPECT_FALSE(error_collector.is_valid());

		using namespace std::string_view_literals;
		static constexpr std::array k_expected_errors = {
			std::make_pair(3, "struct_declaration_error"sv),
			std::make_pair(2, "module_error"sv),
		};
		const auto& result_errors = error_collector.errors();
		ASSERT_EQ(result_errors.size(), k_expected_errors.size());
		for (std::size_t index = 0; index < result_errors.size(); ++index) {
			const auto& [expected_depth, expected_message] = k_expected_errors[index];
			const auto& [result_depth, error_node]         = result_errors[index];
			EXPECT_EQ(expected_depth, result_depth);
			ASSERT_TRUE(error_node);
			EXPECT_EQ(expected_message, error_node->message);
		}
	}

	TEST_F(ErrorCollectorTest, AllErrors)
	{
		auto struct_declaration_members = ASTNode::Dependencies{};
		struct_declaration_members.reserve(3);
		struct_declaration_members.emplace_back(VariableDeclarationNode::create("my_int", "i32", nullptr, false));
		struct_declaration_members.emplace_back(VariableDeclarationNode::create("my_str", "str", nullptr, false));
		struct_declaration_members.emplace_back(ErrorNode::create("struct_declaration_error"));
		auto struct_declaration = StructDeclarationNode::create("my_struct", std::move(struct_declaration_members));

		auto if_node_true_statements = ASTNode::Dependencies{};
		if_node_true_statements.emplace_back(
			UnaryNode::create(LiteralNode::create(Value{ "index" }), ASTNode::Operator::Decrement));
		auto if_node_false_statements = ASTNode::Dependencies{};
		if_node_false_statements.emplace_back(
			UnaryNode::create(ErrorNode::create("if_false_unary_expr_error"), ASTNode::Operator::Increment));

		auto if_node = IfNode::create(LiteralNode::create(Value{ true }),
		                              BlockNode::create(std::move(if_node_true_statements)),
		                              BlockNode::create(std::move(if_node_false_statements)));

		auto for_loop_initialization
			= VariableDeclarationNode::create("index", "i32", LiteralNode::create(Value{ 0 }), false);
		auto for_loop_condition = BinaryNode::create(LiteralNode::create(Value{ "index" }),
		                                             ErrorNode::create("for_loop_condition_binary_rhs_expr_error"),
		                                             ASTNode::Operator::LessEqual);
		auto for_loop_update = UnaryNode::create(LiteralNode::create(Value{ "index" }), ASTNode::Operator::Increment);
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
		module_statements.push_back(ErrorNode::create("module_error"));
		module_statements.push_back(std::move(function_declaration));
		auto module = ModuleNode::create("error_collector_module", std::move(module_statements));

		ErrorCollectorVisitor error_collector{};
		error_collector.accept(module.get());

		EXPECT_FALSE(error_collector.is_valid());

		using namespace std::string_view_literals;
		static constexpr std::array k_expected_errors = {
			std::make_pair(3, "struct_declaration_error"sv),
			std::make_pair(2, "module_error"sv),
			std::make_pair(6, "for_loop_condition_binary_rhs_expr_error"sv),
			std::make_pair(9, "if_false_unary_expr_error"sv),
		};
		const auto& result_errors = error_collector.errors();
		ASSERT_EQ(result_errors.size(), k_expected_errors.size());
		for (std::size_t index = 0; index < result_errors.size(); ++index) {
			const auto& [expected_depth, expected_message] = k_expected_errors[index];
			const auto& [result_depth, error_node]         = result_errors[index];
			EXPECT_EQ(expected_depth, result_depth);
			ASSERT_TRUE(error_node);
			EXPECT_EQ(expected_message, error_node->message);
		}
	}

}  // namespace soul::ast::visitors::ut
