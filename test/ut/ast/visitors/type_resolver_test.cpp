#include "ast/visitors/type_resolver.h"

#include <gtest/gtest.h>

#include "ast/nodes/binary.h"
#include "ast/nodes/block.h"
#include "ast/nodes/cast.h"
#include "ast/nodes/error.h"
#include "ast/nodes/for_loop.h"
#include "ast/nodes/foreach_loop.h"
#include "ast/nodes/function_call.h"
#include "ast/nodes/function_declaration.h"
#include "ast/nodes/if.h"
#include "ast/nodes/literal.h"
#include "ast/nodes/module.h"
#include "ast/nodes/struct_declaration.h"
#include "ast/nodes/unary.h"
#include "ast/nodes/variable_declaration.h"
#include "ast/visitors/error_collector.h"

#include <string_view>
#include <unordered_map>

namespace soul::ast::visitors::ut
{
	using namespace ast::nodes;
	using namespace std::string_view_literals;
	using namespace types;

	class TypeResolverTest : public ::testing::Test
	{
		protected:
		ASTNode::Dependency resolve(ASTNode::Reference root)
		{
			TypeDiscovererVisitor type_discoverer_visitor{};
			type_discoverer_visitor.accept(root);

			auto type_discoverer_root = type_discoverer_visitor.cloned();

			ErrorCollectorVisitor error_collector{};
			error_collector.accept(type_discoverer_root.get());
			if (!error_collector.is_valid()) {
				for (const auto& [depth, error] : error_collector.errors()) {
					std::cerr << std::format("[{}]: {}\n", depth, error->message);
				}
				return nullptr;
			}

			TypeResolverVisitor type_resolver_visitor{ type_discoverer_visitor.discovered_types() };
			type_resolver_visitor.accept(type_discoverer_root.get());

			return type_resolver_visitor.cloned();
		}
	};

	TEST_F(TypeResolverTest, BinaryNode_Arithmetic)
	{
		static constexpr std::array k_arithmetic_operators = { ASTNode::Operator::Add,
			                                                   ASTNode::Operator::Sub,
			                                                   ASTNode::Operator::Mul,
			                                                   ASTNode::Operator::Div,
			                                                   ASTNode::Operator::Mod };

		auto module_statements = ASTNode::Dependencies{};
		module_statements.reserve(k_arithmetic_operators.size());
		for (const auto op : k_arithmetic_operators) {
			module_statements.emplace_back(
				BinaryNode::create(LiteralNode::create(Value{ 1L }, LiteralNode::Type::Int64),
			                       LiteralNode::create(Value{ 5L }, LiteralNode::Type::Int64),
			                       op));
		}
		auto expected_module = ModuleNode::create("resolve_module", std::move(module_statements));

		auto result_module = resolve(expected_module.get());

		EXPECT_NE(expected_module.get(), result_module.get());
		const auto* as_module = dynamic_cast<ModuleNode*>(result_module.get());
		ASSERT_TRUE(as_module);
		ASSERT_EQ(as_module->statements.size(), k_arithmetic_operators.size());

		for (std::size_t index = 0; index < k_arithmetic_operators.size(); ++index) {
			const auto* as_binary = dynamic_cast<BinaryNode*>(as_module->statements[index].get());
			ASSERT_TRUE(as_binary);
			EXPECT_EQ(as_binary->op, k_arithmetic_operators[index]);
			EXPECT_EQ(as_binary->type, PrimitiveType::Kind::Int64);

			const auto* as_lhs = dynamic_cast<LiteralNode*>(as_binary->lhs.get());
			ASSERT_TRUE(as_lhs);
			EXPECT_EQ(as_lhs->value, Value{ 1L });
			EXPECT_EQ(as_lhs->literal_type, LiteralNode::Type::Int64);
			EXPECT_EQ(as_lhs->type, PrimitiveType::Kind::Int64);

			const auto* as_rhs = dynamic_cast<LiteralNode*>(as_binary->rhs.get());
			ASSERT_TRUE(as_rhs);
			EXPECT_EQ(as_rhs->value, Value{ 5L });
			EXPECT_EQ(as_rhs->literal_type, LiteralNode::Type::Int64);
			EXPECT_EQ(as_rhs->type, PrimitiveType::Kind::Int64);
		}
	}

	TEST_F(TypeResolverTest, BinaryNode_Comparison)
	{
		static constexpr std::array k_comparison_operators

			= { ASTNode::Operator::Equal,        ASTNode::Operator::NotEqual, ASTNode::Operator::Greater,
			    ASTNode::Operator::GreaterEqual, ASTNode::Operator::Less,     ASTNode::Operator::LessEqual };

		auto module_statements = ASTNode::Dependencies{};
		module_statements.reserve(k_comparison_operators.size());
		for (const auto op : k_comparison_operators) {
			module_statements.emplace_back(
				BinaryNode::create(LiteralNode::create(Value{ 1.0 }, LiteralNode::Type::Float64),
			                       LiteralNode::create(Value{ 5.0 }, LiteralNode::Type::Float64),
			                       op));
		}
		auto expected_module = ModuleNode::create("resolve_module", std::move(module_statements));

		auto result_module = resolve(expected_module.get());

		EXPECT_NE(expected_module.get(), result_module.get());
		const auto* as_module = dynamic_cast<ModuleNode*>(result_module.get());
		ASSERT_TRUE(as_module);
		ASSERT_EQ(as_module->statements.size(), k_comparison_operators.size());

		for (std::size_t index = 0; index < k_comparison_operators.size(); ++index) {
			const auto* as_binary = dynamic_cast<BinaryNode*>(as_module->statements[index].get());
			ASSERT_TRUE(as_binary) << index;
			EXPECT_EQ(as_binary->op, k_comparison_operators[index]);
			EXPECT_EQ(as_binary->type, PrimitiveType::Kind::Boolean);

			const auto* as_lhs = dynamic_cast<LiteralNode*>(as_binary->lhs.get());
			ASSERT_TRUE(as_lhs);
			EXPECT_EQ(as_lhs->value, Value{ 1.0 });
			EXPECT_EQ(as_lhs->literal_type, LiteralNode::Type::Float64);
			EXPECT_EQ(as_lhs->type, PrimitiveType::Kind::Float64);

			const auto* as_rhs = dynamic_cast<LiteralNode*>(as_binary->rhs.get());
			ASSERT_TRUE(as_rhs);
			EXPECT_EQ(as_rhs->value, Value{ 5.0 });
			EXPECT_EQ(as_rhs->literal_type, LiteralNode::Type::Float64);
			EXPECT_EQ(as_rhs->type, PrimitiveType::Kind::Float64);
		}
	}

	TEST_F(TypeResolverTest, BinaryNode_Logical)
	{
		static constexpr std::array k_logical_operators
			= { ASTNode::Operator::LogicalOr, ASTNode::Operator::LogicalAnd };

		auto module_statements = ASTNode::Dependencies{};
		module_statements.reserve(k_logical_operators.size());
		for (const auto op : k_logical_operators) {
			module_statements.emplace_back(
				BinaryNode::create(LiteralNode::create(Value{ true }, LiteralNode::Type::Boolean),
			                       LiteralNode::create(Value{ false }, LiteralNode::Type::Boolean),
			                       op));
		}
		auto expected_module = ModuleNode::create("resolve_module", std::move(module_statements));

		auto result_module = resolve(expected_module.get());

		EXPECT_NE(expected_module.get(), result_module.get());
		const auto* as_module = dynamic_cast<ModuleNode*>(result_module.get());
		ASSERT_TRUE(as_module);
		ASSERT_EQ(as_module->statements.size(), k_logical_operators.size());

		for (std::size_t index = 0; index < k_logical_operators.size(); ++index) {
			const auto* as_binary = dynamic_cast<BinaryNode*>(as_module->statements[index].get());
			ASSERT_TRUE(as_binary) << index;
			EXPECT_EQ(as_binary->op, k_logical_operators[index]);
			EXPECT_EQ(as_binary->type, PrimitiveType::Kind::Boolean);

			const auto* as_lhs = dynamic_cast<LiteralNode*>(as_binary->lhs.get());
			ASSERT_TRUE(as_lhs);
			EXPECT_EQ(as_lhs->value, Value{ true });
			EXPECT_EQ(as_lhs->literal_type, LiteralNode::Type::Boolean);
			EXPECT_EQ(as_lhs->type, PrimitiveType::Kind::Boolean);

			const auto* as_rhs = dynamic_cast<LiteralNode*>(as_binary->rhs.get());
			ASSERT_TRUE(as_rhs);
			EXPECT_EQ(as_rhs->value, Value{ false });
			EXPECT_EQ(as_rhs->literal_type, LiteralNode::Type::Boolean);
			EXPECT_EQ(as_rhs->type, PrimitiveType::Kind::Boolean);
		}
	}

	TEST_F(TypeResolverTest, BinaryNode_NoOverload)
	{
		auto module_statements = ASTNode::Dependencies{};
		module_statements.emplace_back(
			BinaryNode::create(LiteralNode::create(Value{ "my_string" }, LiteralNode::Type::String),
		                       LiteralNode::create(Value{ 5.0 }, LiteralNode::Type::Float32),
		                       ASTNode::Operator::LogicalAnd));
		auto expected_module = ModuleNode::create("resolve_module", std::move(module_statements));

		auto result_module = resolve(expected_module.get());

		EXPECT_NE(expected_module.get(), result_module.get());
		const auto* as_module = dynamic_cast<ModuleNode*>(result_module.get());
		ASSERT_TRUE(as_module);
		ASSERT_EQ(as_module->statements.size(), 1);

		const auto* as_error = dynamic_cast<ErrorNode*>(as_module->statements[0].get());
		ASSERT_TRUE(as_error);
		EXPECT_EQ(as_error->message,
		          std::format("operator ('{}') does not exist for types '{}' and '{}'",
		                      ASTNode::name(ASTNode::Operator::LogicalAnd),
		                      std::string(Type{ PrimitiveType::Kind::String }),
		                      std::string(Type{ PrimitiveType::Kind::Float32 })));
	}

	TEST_F(TypeResolverTest, BlockNode)
	{
		auto block_node_statements = ASTNode::Dependencies{};
		block_node_statements.emplace_back(VariableDeclarationNode::create(
			"in_scope", "f32", LiteralNode::create(Value{ "before_scope" }, LiteralNode::Type::Identifier), false));

		auto module_statements = ASTNode::Dependencies{};
		module_statements.reserve(3);
		module_statements.emplace_back(VariableDeclarationNode::create(
			"before_scope", "f32", LiteralNode::create(Value{ 1.0 }, LiteralNode::Type::Float32), false));
		module_statements.emplace_back(BlockNode::create(std::move(block_node_statements)));
		module_statements.emplace_back(VariableDeclarationNode::create(
			"after_scope", "i32", LiteralNode::create(Value{ 5 }, LiteralNode::Type::Int32), false));
		auto expected_module = ModuleNode::create("resolve_module", std::move(module_statements));

		auto result_module = resolve(expected_module.get());

		EXPECT_NE(expected_module.get(), result_module.get());
		const auto* as_module = dynamic_cast<ModuleNode*>(result_module.get());
		ASSERT_TRUE(as_module);
		ASSERT_EQ(as_module->statements.size(), 3);

		const auto* as_before_scope_variable = dynamic_cast<VariableDeclarationNode*>(as_module->statements[0].get());
		ASSERT_TRUE(as_before_scope_variable);
		EXPECT_EQ(as_before_scope_variable->name, "before_scope");
		EXPECT_EQ(as_before_scope_variable->type_identifier, "f32");
		EXPECT_TRUE(as_before_scope_variable->expression);
		{
			const auto* as_literal = dynamic_cast<LiteralNode*>(as_before_scope_variable->expression.get());
			ASSERT_TRUE(as_literal);
			EXPECT_EQ(as_literal->value, Value{ 1.0 });
			EXPECT_EQ(as_literal->literal_type, LiteralNode::Type::Float32);
			EXPECT_EQ(as_literal->type, PrimitiveType::Kind::Float32);
		}
		EXPECT_FALSE(as_before_scope_variable->is_mutable);
		EXPECT_EQ(as_before_scope_variable->type, PrimitiveType::Kind::Float32);

		const auto* as_block = dynamic_cast<BlockNode*>(as_module->statements[1].get());
		ASSERT_TRUE(as_block);
		ASSERT_EQ(as_block->statements.size(), 1);
		{
			const auto* as_in_scope_variable = dynamic_cast<VariableDeclarationNode*>(as_block->statements[0].get());
			ASSERT_TRUE(as_in_scope_variable);
			EXPECT_EQ(as_in_scope_variable->name, "in_scope");
			EXPECT_EQ(as_in_scope_variable->type_identifier, "f32");
			EXPECT_TRUE(as_in_scope_variable->expression);
			{
				const auto* as_literal = dynamic_cast<LiteralNode*>(as_in_scope_variable->expression.get());
				ASSERT_TRUE(as_literal);
				EXPECT_EQ(as_literal->value, Value{ "before_scope" });
				EXPECT_EQ(as_literal->literal_type, LiteralNode::Type::Identifier);
				EXPECT_EQ(as_literal->type, PrimitiveType::Kind::Float32);
			}
			EXPECT_FALSE(as_in_scope_variable->is_mutable);
			EXPECT_EQ(as_in_scope_variable->type, PrimitiveType::Kind::Float32);
		}

		const auto* as_after_scope_variable = dynamic_cast<VariableDeclarationNode*>(as_module->statements[2].get());
		ASSERT_TRUE(as_after_scope_variable);
		EXPECT_EQ(as_after_scope_variable->name, "after_scope");
		EXPECT_EQ(as_after_scope_variable->type_identifier, "i32");
		EXPECT_TRUE(as_after_scope_variable->expression);
		{
			const auto* as_literal = dynamic_cast<LiteralNode*>(as_after_scope_variable->expression.get());
			ASSERT_TRUE(as_literal);
			EXPECT_EQ(as_literal->value, Value{ 5 });
			EXPECT_EQ(as_literal->literal_type, LiteralNode::Type::Int32);
			EXPECT_EQ(as_literal->type, PrimitiveType::Kind::Int32);
		}
		EXPECT_FALSE(as_after_scope_variable->is_mutable);
		EXPECT_EQ(as_after_scope_variable->type, PrimitiveType::Kind::Int32);
	}

	TEST_F(TypeResolverTest, Cast_BasicType)
	{
		auto cast_node         = CastNode::create(LiteralNode::create(Value{ 128L }, LiteralNode::Type::Int64), "i32");
		auto module_statements = ASTNode::Dependencies{};
		module_statements.push_back(std::move(cast_node));
		auto expected_module = ModuleNode::create("resolve_module", std::move(module_statements));

		auto result_module = resolve(expected_module.get());

		EXPECT_NE(expected_module.get(), result_module.get());
		const auto* as_module = dynamic_cast<ModuleNode*>(result_module.get());
		ASSERT_TRUE(as_module);
		ASSERT_EQ(as_module->statements.size(), 1);

		const auto* as_cast = dynamic_cast<CastNode*>(as_module->statements[0].get());
		ASSERT_TRUE(as_cast);
		EXPECT_EQ(as_cast->type_identifier, "i32");
		EXPECT_EQ(as_cast->type, PrimitiveType::Kind::Int32);

		const auto* as_literal = dynamic_cast<LiteralNode*>(as_cast->expression.get());
		ASSERT_TRUE(as_literal);
		EXPECT_EQ(as_literal->value, Value{ 128L });
		EXPECT_EQ(as_literal->literal_type, LiteralNode::Type::Int64);
		EXPECT_EQ(as_literal->type, PrimitiveType::Kind::Int64);
	}

	TEST_F(TypeResolverTest, Cast_Impossible)
	{
		auto cast_node         = CastNode::create(LiteralNode::create(Value{ 128L }, LiteralNode::Type::Int64), "chr");
		auto module_statements = ASTNode::Dependencies{};
		module_statements.push_back(std::move(cast_node));
		auto expected_module = ModuleNode::create("resolve_module", std::move(module_statements));

		auto result_module = resolve(expected_module.get());

		EXPECT_NE(expected_module.get(), result_module.get());
		const auto* as_module = dynamic_cast<ModuleNode*>(result_module.get());
		ASSERT_TRUE(as_module);
		ASSERT_EQ(as_module->statements.size(), 1);

		const auto* as_error = dynamic_cast<ErrorNode*>(as_module->statements[0].get());
		ASSERT_TRUE(as_error);
		EXPECT_EQ(as_error->message,
		          std::format("cannot cast from type '{}' to '{}'",
		                      std::string(Type{ PrimitiveType::Kind::Int64 }),
		                      std::string(Type{ PrimitiveType::Kind::Char })));
	}

	TEST_F(TypeResolverTest, ForLoop)
	{
		auto initialization = VariableDeclarationNode::create(
			"index", "i32", LiteralNode::create(Value{ 0 }, LiteralNode::Type::Int32), true);

		auto condition = BinaryNode::create(LiteralNode::create(Value{ "index" }, LiteralNode::Type::Identifier),
		                                    LiteralNode::create(Value{ 10 }, LiteralNode::Type::Int32),
		                                    ASTNode::Operator::Less);

		auto update = UnaryNode::create(LiteralNode::create(Value{ "index" }, LiteralNode::Type::Identifier),
		                                ASTNode::Operator::Increment);

		auto for_loop_statements = ASTNode::Dependencies{};
		for_loop_statements.emplace_back(LiteralNode::create(Value{ "my_string" }, LiteralNode::Type::String));

		auto for_loop          = ForLoopNode::create(std::move(initialization),
                                            std::move(condition),
                                            std::move(update),
                                            BlockNode::create(std::move(for_loop_statements)));
		auto module_statements = ASTNode::Dependencies{};
		module_statements.push_back(std::move(for_loop));
		auto expected_module = ModuleNode::create("resolve_module", std::move(module_statements));

		auto result_module = resolve(expected_module.get());

		EXPECT_NE(expected_module.get(), result_module.get());
		const auto* as_module = dynamic_cast<ModuleNode*>(result_module.get());
		ASSERT_TRUE(as_module);
		ASSERT_EQ(as_module->statements.size(), 1);

		const auto* as_for_loop = dynamic_cast<ForLoopNode*>(as_module->statements[0].get());
		ASSERT_TRUE(as_for_loop);

		const auto* as_initialization = dynamic_cast<VariableDeclarationNode*>(as_for_loop->initialization.get());
		ASSERT_TRUE(as_initialization);
		EXPECT_EQ(as_initialization->type, PrimitiveType::Kind::Int32);
		EXPECT_EQ(as_initialization->name, "index");
		EXPECT_EQ(as_initialization->type_identifier, "i32");
		EXPECT_TRUE(as_initialization->is_mutable);
		ASSERT_TRUE(as_initialization->expression);
		{
			const auto* as_value = dynamic_cast<LiteralNode*>(as_initialization->expression.get());
			ASSERT_TRUE(as_value);
			EXPECT_EQ(as_value->type, PrimitiveType::Kind::Int32);
			EXPECT_EQ(as_value->value, Value{ 0 });
			EXPECT_EQ(as_value->literal_type, LiteralNode::Type::Int32);
		}

		const auto* as_condition = dynamic_cast<BinaryNode*>(as_for_loop->condition.get());
		ASSERT_TRUE(as_condition);
		EXPECT_EQ(as_condition->type, PrimitiveType::Kind::Boolean);
		{
			EXPECT_EQ(as_condition->op, ASTNode::Operator::Less);

			const auto* as_lhs = dynamic_cast<LiteralNode*>(as_condition->lhs.get());
			ASSERT_TRUE(as_lhs);
			EXPECT_EQ(as_lhs->type, PrimitiveType::Kind::Int32);
			EXPECT_EQ(as_lhs->value, Value{ "index" });
			EXPECT_EQ(as_lhs->literal_type, LiteralNode::Type::Identifier);

			const auto* as_rhs = dynamic_cast<LiteralNode*>(as_condition->rhs.get());
			ASSERT_TRUE(as_rhs);
			EXPECT_EQ(as_rhs->type, PrimitiveType::Kind::Int32);
			EXPECT_EQ(as_rhs->value, Value{ 10 });
			EXPECT_EQ(as_rhs->literal_type, LiteralNode::Type::Int32);
		}

		const auto* as_update = dynamic_cast<UnaryNode*>(as_for_loop->update.get());
		ASSERT_TRUE(as_update);
		EXPECT_EQ(as_update->type, PrimitiveType::Kind::Int32);
		{
			EXPECT_EQ(as_update->op, ASTNode::Operator::Increment);

			const auto* as_value = dynamic_cast<LiteralNode*>(as_update->expression.get());
			ASSERT_TRUE(as_value);
			EXPECT_EQ(as_value->type, PrimitiveType::Kind::Int32);
			EXPECT_EQ(as_value->value, Value{ "index" });
			EXPECT_EQ(as_value->literal_type, LiteralNode::Type::Identifier);
		}

		const auto* as_statements = dynamic_cast<BlockNode*>(as_for_loop->statements.get());
		ASSERT_TRUE(as_statements);
		ASSERT_EQ(as_statements->statements.size(), 1);
	}

	TEST_F(TypeResolverTest, ForLoop_ConditionNotBool)
	{
		auto condition = BinaryNode::create(LiteralNode::create(Value{ "index" }, LiteralNode::Type::Identifier),
		                                    LiteralNode::create(Value{ 10 }, LiteralNode::Type::Int32),
		                                    ASTNode::Operator::Add);
		auto for_loop
			= ForLoopNode::create(nullptr, std::move(condition), nullptr, BlockNode::create(ASTNode::Dependencies{}));
		auto module_statements = ASTNode::Dependencies{};
		module_statements.push_back(std::move(for_loop));
		auto expected_module = ModuleNode::create("resolve_module", std::move(module_statements));

		auto result_module = resolve(expected_module.get());

		EXPECT_NE(expected_module.get(), result_module.get());
		const auto* as_module = dynamic_cast<ModuleNode*>(result_module.get());
		ASSERT_TRUE(as_module);
		ASSERT_EQ(as_module->statements.size(), 1);

		const auto* as_error = dynamic_cast<ErrorNode*>(as_module->statements[0].get());
		ASSERT_TRUE(as_error);
		EXPECT_EQ(as_error->message,
		          std::format("condition in for loop statement must be convertible to a '{}' type",
		                      std::string(Type{ PrimitiveType::Kind::Boolean })));
	}

	TEST_F(TypeResolverTest, FunctionCallNode)
	{
		static constexpr auto k_function_name = "my_function";

		auto function_declaration_parameters = ASTNode::Dependencies{};
		function_declaration_parameters.emplace_back(VariableDeclarationNode::create("a", "str", nullptr, false));
		auto function_declaration = FunctionDeclarationNode::create(k_function_name,
		                                                            "f32",
		                                                            std::move(function_declaration_parameters),
		                                                            BlockNode::create(ASTNode::Dependencies{}));

		auto function_call_parameters = ASTNode::Dependencies{};
		function_call_parameters.emplace_back(LiteralNode::create(Value{ "some_string" }, LiteralNode::Type::String));
		auto function_call     = FunctionCallNode::create(k_function_name, ASTNode::Dependencies{});
		auto module_statements = ASTNode::Dependencies{};
		module_statements.push_back(std::move(function_call));
		auto expected_module = ModuleNode::create("resolve_module", std::move(module_statements));

		auto result_module = resolve(expected_module.get());

		EXPECT_NE(expected_module.get(), result_module.get());
		const auto* as_module = dynamic_cast<ModuleNode*>(result_module.get());
		ASSERT_TRUE(as_module);
		ASSERT_EQ(as_module->statements.size(), 1);

		const auto* as_error = dynamic_cast<ErrorNode*>(as_module->statements[0].get());
		ASSERT_TRUE(as_error);
		EXPECT_EQ(as_error->message, std::format("cannot call non-existing function '{}'", k_function_name));
	}

	TEST_F(TypeResolverTest, FunctionCallNode_NonExistingFunction)
	{
		auto function_call     = FunctionCallNode::create("non_existing", ASTNode::Dependencies{});
		auto module_statements = ASTNode::Dependencies{};
		module_statements.push_back(std::move(function_call));
		auto expected_module = ModuleNode::create("resolve_module", std::move(module_statements));

		auto result_module = resolve(expected_module.get());

		EXPECT_NE(expected_module.get(), result_module.get());
		const auto* as_module = dynamic_cast<ModuleNode*>(result_module.get());
		ASSERT_TRUE(as_module);
		ASSERT_EQ(as_module->statements.size(), 1);

		const auto* as_error = dynamic_cast<ErrorNode*>(as_module->statements[0].get());
		ASSERT_TRUE(as_error);
		EXPECT_EQ(as_error->message, "cannot call non-existing function 'non_existing'");
	}

	TEST_F(TypeResolverTest, FunctionCallNode_ParametersDoNotMatch)
	{
		static constexpr auto k_function_name = "my_function";

		auto function_declaration_parameters = ASTNode::Dependencies{};
		function_declaration_parameters.emplace_back(VariableDeclarationNode::create("a", "str", nullptr, false));
		auto function_declaration = FunctionDeclarationNode::create(k_function_name,
		                                                            "f32",
		                                                            std::move(function_declaration_parameters),
		                                                            BlockNode::create(ASTNode::Dependencies{}));

		auto function_call_parameters = ASTNode::Dependencies{};
		function_call_parameters.emplace_back(LiteralNode::create(Value{ "some_string" }, LiteralNode::Type::String));
		auto function_call     = FunctionCallNode::create(k_function_name, ASTNode::Dependencies{});
		auto module_statements = ASTNode::Dependencies{};
		module_statements.reserve(2);
		module_statements.push_back(std::move(function_declaration));
		module_statements.push_back(std::move(function_call));
		auto expected_module = ModuleNode::create("resolve_module", std::move(module_statements));

		auto result_module = resolve(expected_module.get());

		EXPECT_NE(expected_module.get(), result_module.get());
		const auto* as_module = dynamic_cast<ModuleNode*>(result_module.get());
		ASSERT_TRUE(as_module);
		ASSERT_EQ(as_module->statements.size(), 2);

		const auto* as_function_declaration = dynamic_cast<FunctionDeclarationNode*>(as_module->statements[0].get());
		ASSERT_TRUE(as_function_declaration);
		EXPECT_EQ(as_function_declaration->name, k_function_name);
		EXPECT_EQ(as_function_declaration->type_identifier, "f32");
		EXPECT_EQ(as_function_declaration->parameters.size(), 1);
		EXPECT_TRUE(as_function_declaration->statements->statements.empty());
		{
			const auto* as_parameter
				= dynamic_cast<VariableDeclarationNode*>(as_function_declaration->parameters[0].get());
			ASSERT_TRUE(as_parameter);
			EXPECT_EQ(as_parameter->name, "a");
			EXPECT_EQ(as_parameter->type_identifier, "str");
			EXPECT_FALSE(as_parameter->expression);
			EXPECT_FALSE(as_parameter->is_mutable);
		}

		const auto* as_error = dynamic_cast<ErrorNode*>(as_module->statements[1].get());
		ASSERT_TRUE(as_error);
		EXPECT_EQ(as_error->message, std::format("cannot call non-existing function '{}'", k_function_name));
	}

	TEST_F(TypeResolverTest, FunctionDeclarationNode)
	{
		auto function_declaration_parameters = ASTNode::Dependencies{};
		function_declaration_parameters.reserve(3);
		function_declaration_parameters.emplace_back(VariableDeclarationNode::create("a", "i32", nullptr, false));
		function_declaration_parameters.emplace_back(VariableDeclarationNode::create("b", "f64", nullptr, false));
		function_declaration_parameters.emplace_back(VariableDeclarationNode::create("c", "chr", nullptr, false));
		auto function_declaration_statements = ASTNode::Dependencies{};
		function_declaration_statements.emplace_back(VariableDeclarationNode::create(
			"d", "chr", LiteralNode::create(Value{ "c" }, LiteralNode::Type::Identifier), false));
		auto function_declaration
			= FunctionDeclarationNode::create("my_function",
		                                      "str",
		                                      std::move(function_declaration_parameters),
		                                      BlockNode::create(std::move(function_declaration_statements)));

		auto module_statements = ASTNode::Dependencies{};
		module_statements.push_back(std::move(function_declaration));
		auto expected_module = ModuleNode::create("resolve_module", std::move(module_statements));

		auto result_module = resolve(expected_module.get());

		EXPECT_NE(expected_module.get(), result_module.get());
		const auto* as_module = dynamic_cast<ModuleNode*>(result_module.get());
		ASSERT_TRUE(as_module);
		ASSERT_EQ(as_module->statements.size(), 1);

		const auto* as_function_declaration = dynamic_cast<FunctionDeclarationNode*>(as_module->statements[0].get());
		ASSERT_TRUE(as_function_declaration);
		EXPECT_EQ(as_function_declaration->name, "my_function");
		EXPECT_EQ(as_function_declaration->type_identifier, "str");
		ASSERT_EQ(as_function_declaration->parameters.size(), 3);
		{
			const auto* as_variable_declaration
				= dynamic_cast<VariableDeclarationNode*>(as_function_declaration->parameters[0].get());
			ASSERT_TRUE(as_variable_declaration);
			EXPECT_EQ(as_variable_declaration->name, "a");
			EXPECT_EQ(as_variable_declaration->type_identifier, "i32");
			EXPECT_FALSE(as_variable_declaration->expression);
			EXPECT_FALSE(as_variable_declaration->is_mutable);
			EXPECT_EQ(as_variable_declaration->type, PrimitiveType::Kind::Int32);
		}
		{
			const auto* as_variable_declaration
				= dynamic_cast<VariableDeclarationNode*>(as_function_declaration->parameters[1].get());
			ASSERT_TRUE(as_variable_declaration);
			EXPECT_EQ(as_variable_declaration->name, "b");
			EXPECT_EQ(as_variable_declaration->type_identifier, "f64");
			EXPECT_FALSE(as_variable_declaration->expression);
			EXPECT_FALSE(as_variable_declaration->is_mutable);
			EXPECT_EQ(as_variable_declaration->type, PrimitiveType::Kind::Float64);
		}
		{
			const auto* as_variable_declaration
				= dynamic_cast<VariableDeclarationNode*>(as_function_declaration->parameters[2].get());
			ASSERT_TRUE(as_variable_declaration);
			EXPECT_EQ(as_variable_declaration->name, "c");
			EXPECT_EQ(as_variable_declaration->type_identifier, "chr");
			EXPECT_FALSE(as_variable_declaration->expression);
			EXPECT_FALSE(as_variable_declaration->is_mutable);
			EXPECT_EQ(as_variable_declaration->type, PrimitiveType::Kind::Char);
		}
		const auto* as_statements = dynamic_cast<BlockNode*>(as_function_declaration->statements.get());
		ASSERT_TRUE(as_statements);
		ASSERT_EQ(as_statements->statements.size(), 1);

		const auto* as_variable_declaration
			= dynamic_cast<VariableDeclarationNode*>(as_statements->statements[0].get());
		ASSERT_TRUE(as_variable_declaration);
		EXPECT_EQ(as_variable_declaration->name, "d");
		EXPECT_EQ(as_variable_declaration->type_identifier, "chr");
		EXPECT_TRUE(as_variable_declaration->expression);
		{
			const auto* as_literal = dynamic_cast<LiteralNode*>(as_variable_declaration->expression.get());
			ASSERT_TRUE(as_literal);
			EXPECT_EQ(as_literal->value, Value{ "c" });
			EXPECT_EQ(as_literal->literal_type, LiteralNode::Type::Identifier);
			EXPECT_EQ(as_literal->type, PrimitiveType::Kind::Char);
		}
		EXPECT_FALSE(as_variable_declaration->is_mutable);
		EXPECT_EQ(as_variable_declaration->type, PrimitiveType::Kind::Char);
	}

	TEST_F(TypeResolverTest, FunctionDeclarationNode_ShadowsPreviousParameter)
	{
		auto function_declaration_parameters = ASTNode::Dependencies{};
		function_declaration_parameters.reserve(2);
		function_declaration_parameters.emplace_back(VariableDeclarationNode::create("a", "i32", nullptr, false));
		function_declaration_parameters.emplace_back(VariableDeclarationNode::create("a", "str", nullptr, false));
		auto function_declaration = FunctionDeclarationNode::create("my_function",
		                                                            "i32",
		                                                            std::move(function_declaration_parameters),
		                                                            BlockNode::create(ASTNode::Dependencies{}));

		auto module_statements = ASTNode::Dependencies{};
		module_statements.push_back(std::move(function_declaration));
		auto expected_module = ModuleNode::create("resolve_module", std::move(module_statements));

		auto result_module = resolve(expected_module.get());

		EXPECT_NE(expected_module.get(), result_module.get());
		const auto* as_module = dynamic_cast<ModuleNode*>(result_module.get());
		ASSERT_TRUE(as_module);
		ASSERT_EQ(as_module->statements.size(), 1);

		const auto* as_function_declaration = dynamic_cast<FunctionDeclarationNode*>(as_module->statements[0].get());
		ASSERT_TRUE(as_function_declaration);
		ASSERT_EQ(as_function_declaration->parameters.size(), 2);

		const auto* as_parameter = dynamic_cast<VariableDeclarationNode*>(as_function_declaration->parameters[0].get());
		ASSERT_TRUE(as_parameter);
		EXPECT_EQ(as_parameter->name, "a");
		EXPECT_EQ(as_parameter->type_identifier, "i32");
		EXPECT_FALSE(as_parameter->expression);
		EXPECT_FALSE(as_parameter->is_mutable);

		const auto* as_error = dynamic_cast<ErrorNode*>(as_function_declaration->parameters[1].get());
		ASSERT_TRUE(as_error);
		EXPECT_EQ(as_error->message, "variable declaration 'a' shadows previous one");
	}

	TEST_F(TypeResolverTest, FunctionDeclarationNode_ShadowsPreviousOne)
	{
		static constexpr auto k_function_name = "my_function";

		auto first_function_declaration = FunctionDeclarationNode::create(
			k_function_name, "i32", ASTNode::Dependencies{}, BlockNode::create(ASTNode::Dependencies{}));
		auto second_function_declaration = FunctionDeclarationNode::create(
			k_function_name, "i32", ASTNode::Dependencies{}, BlockNode::create(ASTNode::Dependencies{}));

		auto module_statements = ASTNode::Dependencies{};
		module_statements.reserve(2);
		module_statements.push_back(std::move(first_function_declaration));
		module_statements.push_back(std::move(second_function_declaration));
		auto expected_module = ModuleNode::create("resolve_module", std::move(module_statements));

		auto result_module = resolve(expected_module.get());

		EXPECT_NE(expected_module.get(), result_module.get());
		const auto* as_module = dynamic_cast<ModuleNode*>(result_module.get());
		ASSERT_TRUE(as_module);
		ASSERT_EQ(as_module->statements.size(), 2);

		const auto* as_function_declaration = dynamic_cast<FunctionDeclarationNode*>(as_module->statements[0].get());
		ASSERT_TRUE(as_function_declaration);
		EXPECT_EQ(as_function_declaration->name, k_function_name);
		EXPECT_EQ(as_function_declaration->type_identifier, "i32");
		EXPECT_EQ(as_function_declaration->parameters.size(), 0);
		EXPECT_EQ(as_function_declaration->statements->statements.size(), 0);
		EXPECT_EQ(as_function_declaration->type, PrimitiveType::Kind::Int32);

		const auto* as_error = dynamic_cast<ErrorNode*>(as_module->statements[1].get());
		ASSERT_TRUE(as_error);
		EXPECT_EQ(as_error->message, std::format("function declaration '{}' shadows previous one", k_function_name));
	}

	TEST_F(TypeResolverTest, FunctionDeclarationNode_ShouldntShadowWithDifferentArguments)
	{
		static constexpr auto k_function_name = "my_function";

		auto first_function_declaration_parameters = ASTNode::Dependencies{};
		first_function_declaration_parameters.emplace_back(VariableDeclarationNode::create("a", "i32", nullptr, false));
		auto first_function_declaration
			= FunctionDeclarationNode::create(k_function_name,
		                                      "i32",
		                                      std::move(first_function_declaration_parameters),
		                                      BlockNode::create(ASTNode::Dependencies{}));

		auto second_function_declaration_parameters = ASTNode::Dependencies{};
		second_function_declaration_parameters.emplace_back(
			VariableDeclarationNode::create("a", "f32", nullptr, false));
		auto second_function_declaration
			= FunctionDeclarationNode::create(k_function_name,
		                                      "i32",
		                                      std::move(second_function_declaration_parameters),
		                                      BlockNode::create(ASTNode::Dependencies{}));

		auto module_statements = ASTNode::Dependencies{};
		module_statements.reserve(2);
		module_statements.push_back(std::move(first_function_declaration));
		module_statements.push_back(std::move(second_function_declaration));
		auto expected_module = ModuleNode::create("resolve_module", std::move(module_statements));

		auto result_module = resolve(expected_module.get());

		EXPECT_NE(expected_module.get(), result_module.get());
		const auto* as_module = dynamic_cast<ModuleNode*>(result_module.get());
		ASSERT_TRUE(as_module);
		ASSERT_EQ(as_module->statements.size(), 2);

		{
			const auto* as_function_declaration
				= dynamic_cast<FunctionDeclarationNode*>(as_module->statements[0].get());
			ASSERT_TRUE(as_function_declaration);
			EXPECT_EQ(as_function_declaration->name, k_function_name);
			EXPECT_EQ(as_function_declaration->type_identifier, "i32");
			EXPECT_EQ(as_function_declaration->parameters.size(), 1);
			EXPECT_EQ(as_function_declaration->statements->statements.size(), 0);
			EXPECT_EQ(as_function_declaration->type, PrimitiveType::Kind::Int32);

			const auto* as_parameter
				= dynamic_cast<VariableDeclarationNode*>(as_function_declaration->parameters[0].get());
			ASSERT_TRUE(as_parameter);
			EXPECT_EQ(as_parameter->name, "a");
			EXPECT_EQ(as_parameter->type_identifier, "i32");
			EXPECT_FALSE(as_parameter->expression);
			EXPECT_FALSE(as_parameter->is_mutable);
			EXPECT_EQ(as_parameter->type, PrimitiveType::Kind::Int32);
		}
		{
			const auto* as_function_declaration
				= dynamic_cast<FunctionDeclarationNode*>(as_module->statements[1].get());
			ASSERT_TRUE(as_function_declaration);
			EXPECT_EQ(as_function_declaration->name, k_function_name);
			EXPECT_EQ(as_function_declaration->type_identifier, "i32");
			EXPECT_EQ(as_function_declaration->parameters.size(), 1);
			EXPECT_EQ(as_function_declaration->statements->statements.size(), 0);
			EXPECT_EQ(as_function_declaration->type, PrimitiveType::Kind::Int32);

			const auto* as_parameter
				= dynamic_cast<VariableDeclarationNode*>(as_function_declaration->parameters[0].get());
			ASSERT_TRUE(as_parameter);
			EXPECT_EQ(as_parameter->name, "a");
			EXPECT_EQ(as_parameter->type_identifier, "f32");
			EXPECT_FALSE(as_parameter->expression);
			EXPECT_FALSE(as_parameter->is_mutable);
			EXPECT_EQ(as_parameter->type, PrimitiveType::Kind::Float32);
		}
	}

	TEST_F(TypeResolverTest, LiteralNode)
	{
		const auto get_value = [](LiteralNode::Type type) -> Value {
			switch (type) {
				case LiteralNode::Type::Boolean:
					return Value{ true };
				case LiteralNode::Type::Char:
					return Value{ 'c' };
				case LiteralNode::Type::Float32:
					return Value{ 1.0f };
				case LiteralNode::Type::Float64:
					return Value{ 10.0 };
				case LiteralNode::Type::Identifier:
					return Value{ "index" };
				case LiteralNode::Type::Int32:
					return Value{ 1 };
				case LiteralNode::Type::Int64:
					return Value{ 10L };
				case LiteralNode::Type::String:
					return Value{ "my_string" };
				case LiteralNode::Type::Unknown:
				default:
					return Value{};
			}
		};

		static constexpr std::array k_literal_types = {
			LiteralNode::Type::Unknown, LiteralNode::Type::Boolean, LiteralNode::Type::Char,
			LiteralNode::Type::Float32, LiteralNode::Type::Float64, LiteralNode::Type::Identifier,
			LiteralNode::Type::Int32,   LiteralNode::Type::Int64,   LiteralNode::Type::String,
		};
		auto module_statements = ASTNode::Dependencies{};
		module_statements.reserve(k_literal_types.size() + 1);
		module_statements.emplace_back(VariableDeclarationNode::create("index", "f32", nullptr, false));
		for (const auto type : k_literal_types) {
			module_statements.emplace_back(LiteralNode::create(get_value(type), type));
		}
		auto expected_module = ModuleNode::create("resolve_module", std::move(module_statements));

		auto result_module = resolve(expected_module.get());

		EXPECT_NE(expected_module.get(), result_module.get());
		const auto* as_module = dynamic_cast<ModuleNode*>(result_module.get());
		ASSERT_TRUE(as_module);
		ASSERT_EQ(as_module->statements.size(), k_literal_types.size() + 1);

		const auto* as_variable_declaration = dynamic_cast<VariableDeclarationNode*>(as_module->statements[0].get());
		ASSERT_TRUE(as_variable_declaration);
		EXPECT_EQ(as_variable_declaration->name, "index");
		EXPECT_EQ(as_variable_declaration->type_identifier, "f32");
		EXPECT_FALSE(as_variable_declaration->expression);
		EXPECT_FALSE(as_variable_declaration->is_mutable);
		EXPECT_EQ(as_variable_declaration->type, PrimitiveType::Kind::Float32);

		static const std::unordered_map<LiteralNode::Type, PrimitiveType::Kind> k_expected_types = {
			{ LiteralNode::Type::Unknown,    PrimitiveType::Kind::Unknown                                         },
			{ LiteralNode::Type::Boolean,    PrimitiveType::Kind::Boolean                                         },
			{ LiteralNode::Type::Char,       PrimitiveType::Kind::Char                                            },
			{ LiteralNode::Type::Float32,    PrimitiveType::Kind::Float32                                         },
			{ LiteralNode::Type::Float64,    PrimitiveType::Kind::Float64                                         },
			{ LiteralNode::Type::Identifier, PrimitiveType::Kind::Float32 /* has to match variable declaration */ },
			{ LiteralNode::Type::Int32,      PrimitiveType::Kind::Int32                                           },
			{ LiteralNode::Type::Int64,      PrimitiveType::Kind::Int64                                           },
			{ LiteralNode::Type::String,     PrimitiveType::Kind::String                                          },
		};
		for (std::size_t index = 0; index < k_literal_types.size(); ++index) {
			const auto literal_type = k_literal_types[index];

			const auto* as_literal = dynamic_cast<LiteralNode*>(as_module->statements[index + 1].get());
			ASSERT_TRUE(as_literal);
			EXPECT_EQ(as_literal->value, get_value(literal_type));
			EXPECT_EQ(as_literal->literal_type, literal_type);
			EXPECT_EQ(as_literal->type, k_expected_types.at(literal_type));
		}
	}

	TEST_F(TypeResolverTest, LiteralNode_UndeclaredIdentifier)
	{
		static constexpr auto k_variable_name = "undeclared_variable";

		auto module_statements = ASTNode::Dependencies{};
		module_statements.emplace_back(LiteralNode::create(Value{ k_variable_name }, LiteralNode::Type::Identifier));
		auto expected_module = ModuleNode::create("resolve_module", std::move(module_statements));

		auto result_module = resolve(expected_module.get());

		EXPECT_NE(expected_module.get(), result_module.get());
		const auto* as_module = dynamic_cast<ModuleNode*>(result_module.get());
		ASSERT_TRUE(as_module);
		ASSERT_EQ(as_module->statements.size(), 1);

		const auto* as_error = dynamic_cast<ErrorNode*>(as_module->statements[0].get());
		ASSERT_TRUE(as_error);
		EXPECT_EQ(as_error->message, std::format("use of undeclared identifier '{}'", k_variable_name));
	}

	TEST_F(TypeResolverTest, StructDeclarationNode)
	{
		auto struct_declaration_parameters = ASTNode::Dependencies{};
		struct_declaration_parameters.push_back(VariableDeclarationNode::create("a", "i32", nullptr, false));
		struct_declaration_parameters.push_back(VariableDeclarationNode::create("b", "f32", nullptr, false));
		struct_declaration_parameters.push_back(VariableDeclarationNode::create("c", "bool", nullptr, false));
		auto struct_declaration = StructDeclarationNode::create("my_struct", std::move(struct_declaration_parameters));

		auto module_statements = ASTNode::Dependencies{};
		module_statements.push_back(std::move(struct_declaration));
		auto expected_module = ModuleNode::create("resolve_module", std::move(module_statements));

		auto result_module = resolve(expected_module.get());

		EXPECT_NE(expected_module.get(), result_module.get());
		const auto* as_module = dynamic_cast<ModuleNode*>(result_module.get());
		ASSERT_TRUE(as_module);
		ASSERT_EQ(as_module->statements.size(), 1);

		const auto expected_type = Type{ StructType{ {
			PrimitiveType::Kind::Int32,
			PrimitiveType::Kind::Float32,
			PrimitiveType::Kind::Boolean,
		} } };

		const auto* as_struct_declaration = dynamic_cast<StructDeclarationNode*>(as_module->statements[0].get());
		ASSERT_TRUE(as_struct_declaration);
		EXPECT_EQ(as_struct_declaration->name, "my_struct");
		EXPECT_EQ(as_struct_declaration->type, expected_type);
		ASSERT_EQ(as_struct_declaration->parameters.size(), 3);
		{
			const auto* as_parameter
				= dynamic_cast<VariableDeclarationNode*>(as_struct_declaration->parameters[0].get());
			ASSERT_TRUE(as_parameter);
			EXPECT_EQ(as_parameter->name, "a");
			EXPECT_EQ(as_parameter->type_identifier, "i32");
			EXPECT_FALSE(as_parameter->expression);
			EXPECT_FALSE(as_parameter->is_mutable);
		}
		{
			const auto* as_parameter
				= dynamic_cast<VariableDeclarationNode*>(as_struct_declaration->parameters[1].get());
			ASSERT_TRUE(as_parameter);
			EXPECT_EQ(as_parameter->name, "b");
			EXPECT_EQ(as_parameter->type_identifier, "f32");
			EXPECT_FALSE(as_parameter->expression);
			EXPECT_FALSE(as_parameter->is_mutable);
		}
		{
			const auto* as_parameter
				= dynamic_cast<VariableDeclarationNode*>(as_struct_declaration->parameters[2].get());
			ASSERT_TRUE(as_parameter);
			EXPECT_EQ(as_parameter->name, "c");
			EXPECT_EQ(as_parameter->type_identifier, "bool");
			EXPECT_FALSE(as_parameter->expression);
			EXPECT_FALSE(as_parameter->is_mutable);
		}
	}

	TEST_F(TypeResolverTest, UnaryNode_Arithmetic)
	{
		static constexpr std::array k_arithmetic_operators
			= { ASTNode::Operator::Increment, ASTNode::Operator::Decrement };

		auto module_statements = ASTNode::Dependencies{};
		module_statements.reserve(k_arithmetic_operators.size());
		for (const auto op : k_arithmetic_operators) {
			module_statements.emplace_back(
				UnaryNode::create(LiteralNode::create(Value{ 1L }, LiteralNode::Type::Int64), op));
		}
		auto expected_module = ModuleNode::create("resolve_module", std::move(module_statements));

		auto result_module = resolve(expected_module.get());

		EXPECT_NE(expected_module.get(), result_module.get());
		const auto* as_module = dynamic_cast<ModuleNode*>(result_module.get());
		ASSERT_TRUE(as_module);
		ASSERT_EQ(as_module->statements.size(), k_arithmetic_operators.size());

		for (std::size_t index = 0; index < k_arithmetic_operators.size(); ++index) {
			const auto* as_unary = dynamic_cast<UnaryNode*>(as_module->statements[index].get());
			ASSERT_TRUE(as_unary);
			EXPECT_EQ(as_unary->op, k_arithmetic_operators[index]);
			EXPECT_EQ(as_unary->type, PrimitiveType::Kind::Int64);

			const auto* as_expression = dynamic_cast<LiteralNode*>(as_unary->expression.get());
			ASSERT_TRUE(as_expression);
			EXPECT_EQ(as_expression->value, Value{ 1L });
			EXPECT_EQ(as_expression->literal_type, LiteralNode::Type::Int64);
			EXPECT_EQ(as_expression->type, PrimitiveType::Kind::Int64);
		}
	}

	TEST_F(TypeResolverTest, UnaryNode_Logical)
	{
		auto module_statements = ASTNode::Dependencies{};
		module_statements.emplace_back(UnaryNode::create(LiteralNode::create(Value{ true }, LiteralNode::Type::Boolean),
		                                                 ASTNode::Operator::LogicalNot));
		auto expected_module = ModuleNode::create("resolve_module", std::move(module_statements));

		auto result_module = resolve(expected_module.get());

		EXPECT_NE(expected_module.get(), result_module.get());
		const auto* as_module = dynamic_cast<ModuleNode*>(result_module.get());
		ASSERT_TRUE(as_module);
		ASSERT_EQ(as_module->statements.size(), 1);

		const auto* as_unary = dynamic_cast<UnaryNode*>(as_module->statements[0].get());
		ASSERT_TRUE(as_unary);
		EXPECT_EQ(as_unary->op, ASTNode::Operator::LogicalNot);
		EXPECT_EQ(as_unary->type, PrimitiveType::Kind::Boolean);

		const auto* as_expression = dynamic_cast<LiteralNode*>(as_unary->expression.get());
		ASSERT_TRUE(as_expression);
		EXPECT_EQ(as_expression->value, Value{ true });
		EXPECT_EQ(as_expression->literal_type, LiteralNode::Type::Boolean);
		EXPECT_EQ(as_expression->type, PrimitiveType::Kind::Boolean);
	}

	TEST_F(TypeResolverTest, UnaryNode_NoOverload)
	{
		auto module_statements = ASTNode::Dependencies{};
		module_statements.emplace_back(UnaryNode::create(
			LiteralNode::create(Value{ "my_string" }, LiteralNode::Type::String), ASTNode::Operator::LogicalNot));
		auto expected_module = ModuleNode::create("resolve_module", std::move(module_statements));

		auto result_module = resolve(expected_module.get());

		EXPECT_NE(expected_module.get(), result_module.get());
		const auto* as_module = dynamic_cast<ModuleNode*>(result_module.get());
		ASSERT_TRUE(as_module);
		ASSERT_EQ(as_module->statements.size(), 1);

		const auto* as_error = dynamic_cast<ErrorNode*>(as_module->statements[0].get());
		ASSERT_TRUE(as_error);
		EXPECT_EQ(as_error->message,
		          std::format("operator ('{}') does not exist for type '{}'",
		                      ASTNode::name(ASTNode::Operator::LogicalNot),
		                      std::string(Type{ PrimitiveType::Kind::String })));
	}

	TEST_F(TypeResolverTest, VariableDeclaration_ShadowsPreviousOne)
	{
		static constexpr auto k_variable_name = "variable_to_be_shadowed";

		auto first_variable  = VariableDeclarationNode::create(k_variable_name, "f32", nullptr, true);
		auto second_variable = VariableDeclarationNode::create(k_variable_name, "i32", nullptr, false);

		auto module_statements = ASTNode::Dependencies{};
		module_statements.reserve(2);
		module_statements.push_back(std::move(first_variable));
		module_statements.push_back(std::move(second_variable));
		auto expected_module = ModuleNode::create("resolve_module", std::move(module_statements));

		auto result_module = resolve(expected_module.get());

		EXPECT_NE(expected_module.get(), result_module.get());
		const auto* as_module = dynamic_cast<ModuleNode*>(result_module.get());
		ASSERT_TRUE(as_module);
		ASSERT_EQ(as_module->statements.size(), 2);

		const auto* as_variable_declaration = dynamic_cast<VariableDeclarationNode*>(as_module->statements[0].get());
		ASSERT_TRUE(as_variable_declaration);
		EXPECT_EQ(as_variable_declaration->name, k_variable_name);
		EXPECT_EQ(as_variable_declaration->type_identifier, "f32");
		EXPECT_FALSE(as_variable_declaration->expression);
		EXPECT_TRUE(as_variable_declaration->is_mutable);
		EXPECT_EQ(as_variable_declaration->type, PrimitiveType::Kind::Float32);

		const auto* as_error = dynamic_cast<ErrorNode*>(as_module->statements[1].get());
		ASSERT_TRUE(as_error);
		EXPECT_EQ(as_error->message, std::format("variable declaration '{}' shadows previous one", k_variable_name));
	}

	TEST_F(TypeResolverTest, VariableDeclaration_ShadowsOuterScopeOne)
	{
		static constexpr auto k_variable_name = "variable_to_be_shadowed";

		auto first_variable = VariableDeclarationNode::create(k_variable_name, "f32", nullptr, true);

		auto inner_scope_statements = ASTNode::Dependencies{};
		inner_scope_statements.emplace_back(VariableDeclarationNode::create(k_variable_name, "i32", nullptr, false));
		auto inner_scope = BlockNode::create(std::move(inner_scope_statements));

		auto module_statements = ASTNode::Dependencies{};
		module_statements.reserve(2);
		module_statements.push_back(std::move(first_variable));
		module_statements.push_back(std::move(inner_scope));
		auto expected_module = ModuleNode::create("resolve_module", std::move(module_statements));

		auto result_module = resolve(expected_module.get());

		EXPECT_NE(expected_module.get(), result_module.get());
		const auto* as_module = dynamic_cast<ModuleNode*>(result_module.get());
		ASSERT_TRUE(as_module);
		ASSERT_EQ(as_module->statements.size(), 2);

		const auto* as_variable_declaration = dynamic_cast<VariableDeclarationNode*>(as_module->statements[0].get());
		ASSERT_TRUE(as_variable_declaration);
		EXPECT_EQ(as_variable_declaration->name, k_variable_name);
		EXPECT_EQ(as_variable_declaration->type_identifier, "f32");
		EXPECT_FALSE(as_variable_declaration->expression);
		EXPECT_TRUE(as_variable_declaration->is_mutable);
		EXPECT_EQ(as_variable_declaration->type, PrimitiveType::Kind::Float32);

		const auto* as_inner_scope = dynamic_cast<BlockNode*>(as_module->statements[1].get());
		ASSERT_TRUE(as_inner_scope);
		ASSERT_EQ(as_inner_scope->statements.size(), 1);

		const auto* as_error = dynamic_cast<ErrorNode*>(as_inner_scope->statements[0].get());
		ASSERT_TRUE(as_error);
		EXPECT_EQ(as_error->message, std::format("variable declaration '{}' shadows previous one", k_variable_name));
	}

	TEST_F(TypeResolverTest, VariableDeclaration_ShouldntShadowPreviousInnerScope)
	{
		static constexpr auto k_variable_name = "variable_to_be_shadowed";

		auto inner_scope_statements = ASTNode::Dependencies{};
		inner_scope_statements.emplace_back(VariableDeclarationNode::create(k_variable_name, "i32", nullptr, false));
		auto inner_scope = BlockNode::create(std::move(inner_scope_statements));

		auto outer_variable = VariableDeclarationNode::create(k_variable_name, "f32", nullptr, true);

		auto module_statements = ASTNode::Dependencies{};
		module_statements.reserve(2);
		module_statements.push_back(std::move(inner_scope));
		module_statements.push_back(std::move(outer_variable));
		auto expected_module = ModuleNode::create("resolve_module", std::move(module_statements));

		auto result_module = resolve(expected_module.get());

		EXPECT_NE(expected_module.get(), result_module.get());
		const auto* as_module = dynamic_cast<ModuleNode*>(result_module.get());
		ASSERT_TRUE(as_module);
		ASSERT_EQ(as_module->statements.size(), 2);

		const auto* as_inner_scope = dynamic_cast<BlockNode*>(as_module->statements[0].get());
		ASSERT_TRUE(as_inner_scope);
		ASSERT_EQ(as_inner_scope->statements.size(), 1);

		const auto* as_inner_variable = dynamic_cast<VariableDeclarationNode*>(as_inner_scope->statements[0].get());
		ASSERT_TRUE(as_inner_variable);
		ASSERT_TRUE(as_inner_variable);
		EXPECT_EQ(as_inner_variable->name, k_variable_name);
		EXPECT_EQ(as_inner_variable->type_identifier, "i32");
		EXPECT_FALSE(as_inner_variable->expression);
		EXPECT_FALSE(as_inner_variable->is_mutable);
		EXPECT_EQ(as_inner_variable->type, PrimitiveType::Kind::Int32);

		const auto* as_outer_variable = dynamic_cast<VariableDeclarationNode*>(as_module->statements[1].get());
		ASSERT_TRUE(as_outer_variable);
		EXPECT_EQ(as_outer_variable->name, k_variable_name);
		EXPECT_EQ(as_outer_variable->type_identifier, "f32");
		EXPECT_FALSE(as_outer_variable->expression);
		EXPECT_TRUE(as_outer_variable->is_mutable);
		EXPECT_EQ(as_outer_variable->type, PrimitiveType::Kind::Float32);
	}
}  // namespace soul::ast::visitors::ut
