#include <gtest/gtest.h>

#include "ast/nodes/binary.h"
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
#include "ast/visitors/type_resolver.h"

#include <string_view>

namespace soul::ast::visitors::ut
{
	using namespace ast::nodes;
	using namespace std::string_view_literals;
	using namespace types;

	class TypeResolverTest : public ::testing::Test
	{
		protected:
		void resolve(ASTNode::Reference root)
		{
			TypeDiscovererVisitor type_discoverer_visitor{};
			type_discoverer_visitor.accept(root);

			TypeResolverVisitor type_resolver_visitor{ type_discoverer_visitor.get() };
			type_resolver_visitor.accept(root);
		}
	};

	TEST_F(TypeResolverTest, Binary_CompatibleTypes)
	{
		auto       lhs  = LiteralNode::create(Value(3.14f));
		auto       rhs  = LiteralNode::create(Value(static_cast<i64>(125)));
		const auto root = BinaryNode::create(std::move(lhs), std::move(rhs), ASTNodeOperator::Add);

		{
			const auto* as_binary = dynamic_cast<BinaryNode*>(root.get());
			ASSERT_TRUE(as_binary);
			ASSERT_TRUE(as_binary->lhs->type.is<PrimitiveType>());
			ASSERT_TRUE(as_binary->rhs->type.is<PrimitiveType>());
			EXPECT_EQ(as_binary->lhs->type.as<PrimitiveType>().type, PrimitiveType::Kind::Unknown);
			EXPECT_EQ(as_binary->rhs->type.as<PrimitiveType>().type, PrimitiveType::Kind::Unknown);
			EXPECT_EQ(as_binary->type, PrimitiveType::Kind::Unknown);
		}

		resolve(root.get());

		{
			const auto* as_binary = dynamic_cast<BinaryNode*>(root.get());
			ASSERT_TRUE(as_binary);
			ASSERT_TRUE(as_binary->lhs->type.is<PrimitiveType>());
			ASSERT_TRUE(as_binary->rhs->type.is<PrimitiveType>());
			EXPECT_EQ(as_binary->lhs->type.as<PrimitiveType>().type, PrimitiveType::Kind::Float32);
			EXPECT_EQ(as_binary->rhs->type.as<PrimitiveType>().type, PrimitiveType::Kind::Float32);
			EXPECT_EQ(as_binary->type, PrimitiveType::Kind::Float32);
		}
	}

	TEST_F(TypeResolverTest, Binary_IncompatibleTypes)
	{
		auto       lhs  = LiteralNode::create(Value(true));
		auto       rhs  = LiteralNode::create(Value(1.2f));
		const auto root = BinaryNode::create(std::move(lhs), std::move(rhs), ASTNodeOperator::Add);

		{
			const auto* as_binary = dynamic_cast<BinaryNode*>(root.get());
			ASSERT_TRUE(as_binary);
			ASSERT_TRUE(as_binary->lhs->type.is<PrimitiveType>());
			ASSERT_TRUE(as_binary->rhs->type.is<PrimitiveType>());
			EXPECT_EQ(as_binary->lhs->type.as<PrimitiveType>().type, PrimitiveType::Kind::Unknown);
			EXPECT_EQ(as_binary->rhs->type.as<PrimitiveType>().type, PrimitiveType::Kind::Unknown);
			EXPECT_EQ(as_binary->type, PrimitiveType::Kind::Unknown);
		}

		resolve(root.get());

		{
			const auto* as_binary = dynamic_cast<BinaryNode*>(root.get());
			ASSERT_TRUE(as_binary);
			ASSERT_TRUE(as_binary->lhs->type.is<PrimitiveType>());
			ASSERT_TRUE(as_binary->rhs->type.is<PrimitiveType>());
			EXPECT_EQ(as_binary->lhs->type.as<PrimitiveType>().type, PrimitiveType::Kind::Boolean);
			EXPECT_EQ(as_binary->rhs->type.as<PrimitiveType>().type, PrimitiveType::Kind::Float32);
			EXPECT_EQ(as_binary->type, PrimitiveType::Kind::Unknown);
		}
	}

	TEST_F(TypeResolverTest, Binary_SameTypes)
	{
		auto       lhs  = LiteralNode::create(Value(2.1f));
		auto       rhs  = LiteralNode::create(Value(1.2f));
		const auto root = BinaryNode::create(std::move(lhs), std::move(rhs), ASTNodeOperator::Add);

		{
			const auto* as_binary = dynamic_cast<BinaryNode*>(root.get());
			ASSERT_TRUE(as_binary);
			ASSERT_TRUE(as_binary->lhs->type.is<PrimitiveType>());
			ASSERT_TRUE(as_binary->rhs->type.is<PrimitiveType>());
			EXPECT_EQ(as_binary->lhs->type.as<PrimitiveType>().type, PrimitiveType::Kind::Unknown);
			EXPECT_EQ(as_binary->rhs->type.as<PrimitiveType>().type, PrimitiveType::Kind::Unknown);
			EXPECT_EQ(as_binary->type, PrimitiveType::Kind::Unknown);
		}

		resolve(root.get());

		{
			const auto* as_binary = dynamic_cast<BinaryNode*>(root.get());
			ASSERT_TRUE(as_binary);
			ASSERT_TRUE(as_binary->lhs->type.is<PrimitiveType>());
			ASSERT_TRUE(as_binary->rhs->type.is<PrimitiveType>());
			EXPECT_EQ(as_binary->lhs->type.as<PrimitiveType>().type, PrimitiveType::Kind::Float32);
			EXPECT_EQ(as_binary->rhs->type.as<PrimitiveType>().type, PrimitiveType::Kind::Float32);
			EXPECT_EQ(as_binary->type, PrimitiveType::Kind::Float32);
		}
	}

	TEST_F(TypeResolverTest, CastNode)
	{
		auto expr = LiteralNode::create(Value(static_cast<f64>(3.14)));
		ASSERT_TRUE(expr->type.is<PrimitiveType>());
		EXPECT_EQ(expr->type.as<PrimitiveType>().type, PrimitiveType::Kind::Unknown);

		auto cast = CastNode::create(std::move(expr), "i64");
		ASSERT_TRUE(cast->type.is<PrimitiveType>());
		EXPECT_EQ(cast->type.as<PrimitiveType>().type, PrimitiveType::Kind::Unknown);

		resolve(cast.get());

		{
			const auto* as_cast = dynamic_cast<CastNode*>(cast.get());
			ASSERT_TRUE(as_cast);
			ASSERT_TRUE(as_cast->type.is<PrimitiveType>());
			EXPECT_EQ(as_cast->type.as<PrimitiveType>().type, PrimitiveType::Kind::Int64);

			const auto* as_expr = dynamic_cast<LiteralNode*>(as_cast->expression.get());
			ASSERT_TRUE(as_expr);
			ASSERT_TRUE(as_expr->type.is<PrimitiveType>());
			EXPECT_EQ(as_expr->type.as<PrimitiveType>().type, PrimitiveType::Kind::Float64);
		}
	}

	TEST_F(TypeResolverTest, ForLoop)
	{
		static constexpr auto k_statements_count = 7;

		auto initialization = VariableDeclarationNode::create("index", "i3232", LiteralNode::create(Value(0)), true);
		ASSERT_TRUE(initialization->type.is<PrimitiveType>());
		EXPECT_EQ(initialization->type.as<PrimitiveType>().type, PrimitiveType::Kind::Unknown);

		auto condition = BinaryNode::create(
			LiteralNode::create(Value("index")), LiteralNode::create(Value(10)), ASTNodeOperator::Less);
		{
			const auto* as_condition = dynamic_cast<BinaryNode*>(condition.get());
			ASSERT_TRUE(as_condition);
			ASSERT_TRUE(as_condition->type.is<PrimitiveType>());
			EXPECT_EQ(as_condition->type.as<PrimitiveType>().type, PrimitiveType::Kind::Unknown);

			const auto* as_lhs = dynamic_cast<LiteralNode*>(as_condition->lhs.get());
			ASSERT_TRUE(as_lhs);
			ASSERT_TRUE(as_lhs->type.is<PrimitiveType>());
			EXPECT_EQ(as_lhs->type.as<PrimitiveType>().type, PrimitiveType::Kind::Unknown);

			const auto* as_rhs = dynamic_cast<LiteralNode*>(as_condition->lhs.get());
			ASSERT_TRUE(as_rhs);
			ASSERT_TRUE(as_rhs->type.is<PrimitiveType>());
			EXPECT_EQ(as_rhs->type.as<PrimitiveType>().type, PrimitiveType::Kind::Unknown);
		}

		auto increment = UnaryNode::create(LiteralNode::create(Value("index")), ASTNodeOperator::Increment);
		{
			const auto* as_increment = dynamic_cast<UnaryNode*>(increment.get());
			ASSERT_TRUE(as_increment);
			ASSERT_TRUE(as_increment->type.is<PrimitiveType>());
			EXPECT_EQ(as_increment->type.as<PrimitiveType>().type, PrimitiveType::Kind::Unknown);

			const auto* as_expr = dynamic_cast<LiteralNode*>(as_increment->expr.get());
			ASSERT_TRUE(as_expr);
			ASSERT_TRUE(as_expr->type.is<PrimitiveType>());
			ASSERT_EQ(as_expr->type.as<PrimitiveType>().type, PrimitiveType::Kind::Unknown);
		}

		ASTNode::Dependencies statements{};
		statements.reserve(k_statements_count);
		for (std::size_t index = 0; index < k_statements_count; ++index) {
			auto literal = LiteralNode::create(Value(static_cast<i64>(index)));
			ASSERT_TRUE(literal->type.is<PrimitiveType>());
			EXPECT_EQ(literal->type.as<PrimitiveType>().type, PrimitiveType::Kind::Unknown);
			statements.emplace_back(std::move(literal));
		}

		auto for_loop = ForLoopNode::create(
			std::move(initialization), std::move(condition), std::move(increment), std::move(statements));
		ASSERT_TRUE(for_loop->type.is<PrimitiveType>());
		EXPECT_EQ(for_loop->type.as<PrimitiveType>().type, PrimitiveType::Kind::Unknown);

		resolve(for_loop.get());

		{
			const auto* as_for_loop = dynamic_cast<ForLoopNode*>(for_loop.get());
			ASSERT_TRUE(as_for_loop);
			ASSERT_TRUE(as_for_loop->type.is<PrimitiveType>());
			EXPECT_EQ(as_for_loop->type.as<PrimitiveType>().type, PrimitiveType::Kind::Void);

			const auto* as_initialization = dynamic_cast<VariableDeclarationNode*>(as_for_loop->initialization.get());
			ASSERT_TRUE(as_initialization);
			ASSERT_TRUE(as_initialization->type.is<PrimitiveType>());
			EXPECT_EQ(as_initialization->type.as<PrimitiveType>().type, PrimitiveType::Kind::Int32);

			const auto* as_condition = dynamic_cast<BinaryNode*>(as_for_loop->condition.get());
			ASSERT_TRUE(as_condition);
			ASSERT_TRUE(as_condition->type.is<PrimitiveType>());
			EXPECT_EQ(as_condition->type.as<PrimitiveType>().type, PrimitiveType::Kind::Boolean);

			const auto* as_lhs = dynamic_cast<LiteralNode*>(as_condition->lhs.get());
			ASSERT_TRUE(as_lhs);
			ASSERT_TRUE(as_lhs->type.is<PrimitiveType>());
			EXPECT_EQ(as_lhs->type.as<PrimitiveType>().type, PrimitiveType::Kind::String);

			const auto* as_rhs = dynamic_cast<LiteralNode*>(as_condition->lhs.get());
			ASSERT_TRUE(as_rhs);
			ASSERT_TRUE(as_rhs->type.is<PrimitiveType>());
			EXPECT_EQ(as_rhs->type.as<PrimitiveType>().type, PrimitiveType::Kind::Int32);

			const auto* as_increment = dynamic_cast<UnaryNode*>(as_for_loop->update.get());
			ASSERT_TRUE(as_increment);
			ASSERT_TRUE(as_increment->type.is<PrimitiveType>());
			EXPECT_EQ(as_increment->type.as<PrimitiveType>().type, PrimitiveType::Kind::Int32);

			const auto* as_expr = dynamic_cast<LiteralNode*>(as_increment->expr.get());
			ASSERT_TRUE(as_expr);
			ASSERT_TRUE(as_expr->type.is<PrimitiveType>());
			EXPECT_EQ(as_expr->type.as<PrimitiveType>().type, PrimitiveType::Kind::String);

			for (std::size_t index = 0; index < k_statements_count; ++index) {
				const auto* as_statement = dynamic_cast<LiteralNode*>(as_for_loop->statements[index].get());
				ASSERT_TRUE(as_statement);
				ASSERT_TRUE(as_statement->type.is<PrimitiveType>());
				EXPECT_EQ(as_statement->type.as<PrimitiveType>().type, PrimitiveType::Kind::Int64);
			}
		}
	}

	TEST_F(TypeResolverTest, ForeachLoop)
	{
		static constexpr auto k_statements_count = 4;

		auto variable = VariableDeclarationNode::create("my_var", "i3232", LiteralNode::create(Value(1)), true);
		ASSERT_TRUE(variable->type.is<PrimitiveType>());
		EXPECT_EQ(variable->type.as<PrimitiveType>().type, PrimitiveType::Kind::Unknown);

		auto in_expression = LiteralNode::create(Value("my_vector"));
		ASSERT_TRUE(in_expression->type.is<PrimitiveType>());
		EXPECT_EQ(in_expression->type.as<PrimitiveType>().type, PrimitiveType::Kind::Unknown);

		ASTNode::Dependencies statements{};
		statements.reserve(k_statements_count);
		for (std::size_t index = 0; index < k_statements_count; ++index) {
			auto literal = LiteralNode::create(Value(static_cast<i64>(index)));
			ASSERT_TRUE(literal->type.is<PrimitiveType>());
			EXPECT_EQ(literal->type.as<PrimitiveType>().type, PrimitiveType::Kind::Unknown);
			statements.emplace_back(std::move(literal));
		}

		auto foreach = ForeachLoopNode::create(std::move(variable), std::move(in_expression), std::move(statements));
		ASSERT_TRUE(foreach->type.is<PrimitiveType>());
		EXPECT_EQ(foreach->type.as<PrimitiveType>().type, PrimitiveType::Kind::Unknown);

		resolve(foreach.get());

		{
			const auto* as_foreach = dynamic_cast<ForeachLoopNode*>(foreach.get());
			ASSERT_TRUE(as_foreach);
			ASSERT_TRUE(as_foreach->type.is<PrimitiveType>());
			EXPECT_EQ(as_foreach->type.as<PrimitiveType>().type, PrimitiveType::Kind::Void);

			const auto* as_variable = dynamic_cast<VariableDeclarationNode*>(as_foreach->variable.get());
			ASSERT_TRUE(as_variable);
			ASSERT_TRUE(as_variable->type.is<PrimitiveType>());
			EXPECT_EQ(as_variable->type.as<PrimitiveType>().type, PrimitiveType::Kind::Int32);

			const auto* as_in_expression = dynamic_cast<LiteralNode*>(as_foreach->in_expression.get());
			ASSERT_TRUE(as_in_expression);
			ASSERT_TRUE(as_in_expression->type.is<PrimitiveType>());
			EXPECT_EQ(as_in_expression->type.as<PrimitiveType>().type, PrimitiveType::Kind::String);

			for (std::size_t index = 0; index < k_statements_count; ++index) {
				const auto* as_statement = dynamic_cast<LiteralNode*>(as_foreach->statements[index].get());
				ASSERT_TRUE(as_statement);
				ASSERT_TRUE(as_statement->type.is<PrimitiveType>());
				EXPECT_EQ(as_statement->type.as<PrimitiveType>().type, PrimitiveType::Kind::Int64);
			}
		}
	}

	TEST_F(TypeResolverTest, FunctionDeclaration)
	{
		static constexpr auto k_parameters_count = 3;
		ASTNode::Dependencies parameters{};
		parameters.reserve(k_parameters_count);
		for (std::size_t index = 0; index < k_parameters_count; ++index) {
			auto param = VariableDeclarationNode::create("", "f64", nullptr, false);
			ASSERT_TRUE(param->type.is<PrimitiveType>());
			EXPECT_EQ(param->type.as<PrimitiveType>().type, PrimitiveType::Kind::Unknown);
			parameters.push_back(std::move(param));
		}

		static constexpr auto k_statements_count = 5;
		ASTNode::Dependencies statements{};
		statements.reserve(k_statements_count);
		for (std::size_t index = 0; index < k_statements_count; ++index) {
			auto statement = LiteralNode::create(Value(static_cast<f32>(index) / 10.0f));
			ASSERT_TRUE(statement->type.is<PrimitiveType>());
			EXPECT_EQ(statement->type.as<PrimitiveType>().type, PrimitiveType::Kind::Unknown);
			statements.push_back(std::move(statement));
		}

		auto function_declaration
			= FunctionDeclarationNode::create("my_function", "i32", std::move(parameters), std::move(statements));
		ASSERT_TRUE(function_declaration->type.is<PrimitiveType>());
		EXPECT_EQ(function_declaration->type.as<PrimitiveType>().type, PrimitiveType::Kind::Unknown);

		resolve(function_declaration.get());

		{
			const auto* as_function_declaration = dynamic_cast<FunctionDeclarationNode*>(function_declaration.get());
			ASSERT_TRUE(as_function_declaration);
			ASSERT_TRUE(as_function_declaration->type.is<PrimitiveType>());
			EXPECT_EQ(as_function_declaration->type.as<PrimitiveType>().type, PrimitiveType::Kind::Int32);

			ASSERT_EQ(as_function_declaration->parameters.size(), k_parameters_count);
			for (const auto* param : as_function_declaration->parameters
			                             | std::views::transform([](const auto& arg) { return arg.get(); })) {
				ASSERT_TRUE(param->type.is<PrimitiveType>());
				EXPECT_EQ(param->type.as<PrimitiveType>().type, PrimitiveType::Kind::Float64);
			}

			ASSERT_EQ(as_function_declaration->statements.size(), k_statements_count);
			for (const auto* statement : as_function_declaration->statements
			                                 | std::views::transform([](const auto& arg) { return arg.get(); })) {
				ASSERT_TRUE(statement->type.is<PrimitiveType>());
				EXPECT_EQ(statement->type.as<PrimitiveType>().type, PrimitiveType::Kind::Float32);
			}
		}
	}

	TEST_F(TypeResolverTest, If)
	{
		static constexpr auto k_statements_count = 4;

		auto condition = BinaryNode::create(
			LiteralNode::create(Value("index")), LiteralNode::create(Value(10)), ASTNodeOperator::GreaterEqual);
		ASSERT_TRUE(condition->type.is<PrimitiveType>());
		EXPECT_EQ(condition->type.as<PrimitiveType>().type, PrimitiveType::Kind::Unknown);

		ASTNode::Dependencies if_statement{};
		if_statement.reserve(k_statements_count);
		for (std::size_t index = 0; index < k_statements_count; ++index) {
			auto statement = LiteralNode::create(Value(static_cast<i64>(index)));
			ASSERT_TRUE(statement->type.is<PrimitiveType>());
			EXPECT_EQ(statement->type.as<PrimitiveType>().type, PrimitiveType::Kind::Unknown);

			if_statement.push_back(std::move(statement));
		}

		ASTNode::Dependencies else_statement{};
		else_statement.reserve(k_statements_count);
		for (std::size_t index = 0; index < k_statements_count; ++index) {
			auto statement = LiteralNode::create(Value(static_cast<f32>(index)));
			ASSERT_TRUE(statement->type.is<PrimitiveType>());
			EXPECT_EQ(statement->type.as<PrimitiveType>().type, PrimitiveType::Kind::Unknown);
			else_statement.push_back(std::move(statement));
		}

		auto if_node = IfNode::create(std::move(condition), std::move(if_statement), std::move(else_statement));
		ASSERT_TRUE(if_node->type.is<PrimitiveType>());
		EXPECT_EQ(if_node->type.as<PrimitiveType>().type, PrimitiveType::Kind::Unknown);

		resolve(if_node.get());

		{
			const auto* as_if_node = dynamic_cast<IfNode*>(if_node.get());
			ASSERT_TRUE(as_if_node);
			ASSERT_TRUE(as_if_node->type.is<PrimitiveType>());
			EXPECT_EQ(as_if_node->type.as<PrimitiveType>().type, PrimitiveType::Kind::Void);

			ASSERT_EQ(as_if_node->if_statements.size(), k_statements_count);
			for (std::size_t index = 0; index < k_statements_count; ++index) {
				const auto& statement = as_if_node->if_statements[index];
				ASSERT_TRUE(statement->type.is<PrimitiveType>());
				EXPECT_EQ(statement->type.as<PrimitiveType>().type, PrimitiveType::Kind::Int64);
			}

			ASSERT_EQ(as_if_node->else_statements.size(), k_statements_count);
			for (std::size_t index = 0; index < k_statements_count; ++index) {
				const auto& statement = as_if_node->else_statements[index];
				ASSERT_TRUE(statement->type.is<PrimitiveType>());
				EXPECT_EQ(statement->type.as<PrimitiveType>().type, PrimitiveType::Kind::Float32);
			}
		}
	}

	TEST_F(TypeResolverTest, Literal)
	{
		const std::array k_input_literals{
			LiteralNode::create(Value{}),
			LiteralNode::create(Value{ true }),
			LiteralNode::create(Value{ false }),
			LiteralNode::create(Value{ static_cast<i32>(1) }),
			LiteralNode::create(Value{ static_cast<i64>(2) }),
			LiteralNode::create(Value{ static_cast<f32>(1.0) }),
			LiteralNode::create(Value{ static_cast<f64>(2.0) }),
			LiteralNode::create(Value{ 'c' }),
			LiteralNode::create(Value{ "my_string" }),
		};

		const std::array k_expected_types{
			PrimitiveType::Kind::Unknown, PrimitiveType::Kind::Boolean, PrimitiveType::Kind::Boolean,
			PrimitiveType::Kind::Int32,   PrimitiveType::Kind::Int64,   PrimitiveType::Kind::Float32,
			PrimitiveType::Kind::Float64, PrimitiveType::Kind::Char,    PrimitiveType::Kind::String,
		};
		ASSERT_EQ(k_input_literals.size(), k_expected_types.size());

		for (std::size_t index = 0; index < k_input_literals.size(); ++index) {
			const auto& literal = k_input_literals[index];

			ASSERT_TRUE(literal->type.is<PrimitiveType>());
			EXPECT_EQ(literal->type.as<PrimitiveType>().type, PrimitiveType::Kind::Unknown);

			resolve(literal.get());

			ASSERT_TRUE(literal->type.is<PrimitiveType>());
			EXPECT_EQ(literal->type.as<PrimitiveType>().type, k_expected_types[index]);
		}
	}

	TEST_F(TypeResolverTest, Module)
	{
		static constexpr auto k_statements_count = 12;

		ASTNode::Dependencies statements{};
		statements.reserve(k_statements_count);
		for (std::size_t index = 0; index < k_statements_count; ++index) {
			statements.emplace_back(LiteralNode::create(Value{ std::to_string(index) }));
			ASSERT_TRUE(statements.back()->type.is<PrimitiveType>());
			EXPECT_EQ(statements.back()->type.as<PrimitiveType>().type, PrimitiveType::Kind::Unknown);
		}

		auto module = ModuleNode::create("my_module", std::move(statements));

		resolve(module.get());

		{
			const auto* as_module = dynamic_cast<ModuleNode*>(module.get());
			ASSERT_TRUE(as_module);
			ASSERT_TRUE(as_module->type.is<PrimitiveType>());
			EXPECT_EQ(as_module->type.as<PrimitiveType>().type, PrimitiveType::Kind::Void);

			ASSERT_EQ(as_module->statements.size(), k_statements_count);
			for (std::size_t index = 0; index < k_statements_count; ++index) {
				const auto& statement = as_module->statements[index];
				ASSERT_TRUE(statement->type.is<PrimitiveType>());
				EXPECT_EQ(statement->type.as<PrimitiveType>().type, PrimitiveType::Kind::String);
			}
		}
	}

	TEST_F(TypeResolverTest, Struct)
	{
		auto param_a = VariableDeclarationNode::create("param_a", "i32", nullptr, false);
		ASSERT_TRUE(param_a->type.is<PrimitiveType>());
		EXPECT_EQ(param_a->type.as<PrimitiveType>().type, PrimitiveType::Kind::Unknown);

		auto param_b = VariableDeclarationNode::create("param_b", "f32", nullptr, false);
		ASSERT_TRUE(param_b->type.is<PrimitiveType>());
		EXPECT_EQ(param_b->type.as<PrimitiveType>().type, PrimitiveType::Kind::Unknown);

		auto param_c = VariableDeclarationNode::create("param_c", "str", nullptr, false);
		ASSERT_TRUE(param_c->type.is<PrimitiveType>());
		EXPECT_EQ(param_c->type.as<PrimitiveType>().type, PrimitiveType::Kind::Unknown);

		ASTNode::Dependencies members{};
		members.push_back(std::move(param_a));
		members.push_back(std::move(param_b));
		members.push_back(std::move(param_c));

		auto struct_declaration = StructDeclarationNode::create("my_struct", std::move(members));
		ASSERT_TRUE(struct_declaration->type.is<PrimitiveType>());
		EXPECT_EQ(struct_declaration->type.as<PrimitiveType>().type, PrimitiveType::Kind::Unknown);

		resolve(struct_declaration.get());

		static const auto k_expected_type = StructType{
			{
             PrimitiveType::Kind::Int32,
             PrimitiveType::Kind::Float32,
             PrimitiveType::Kind::String,
			 }
		};

		ASSERT_TRUE(struct_declaration->type.is<StructType>())
			<< "Expected Struct type, but got: " << std::string(struct_declaration->type);
		EXPECT_EQ(struct_declaration->type.as<StructType>().types, k_expected_type);
	}

	TEST_F(TypeResolverTest, Unary)
	{
		auto expr = LiteralNode::create(Value{ "index" });
		ASSERT_TRUE(expr->type.is<PrimitiveType>());
		EXPECT_EQ(expr->type.as<PrimitiveType>().type, PrimitiveType::Kind::Unknown);

		auto unary_node = UnaryNode::create(std::move(expr), ASTNodeOperator::Decrement);
		ASSERT_TRUE(unary_node->type.is<PrimitiveType>());
		EXPECT_EQ(unary_node->type.as<PrimitiveType>().type, PrimitiveType::Kind::Unknown);

		resolve(unary_node.get());

		ASSERT_TRUE(unary_node->type.is<PrimitiveType>());
		EXPECT_EQ(unary_node->type.as<PrimitiveType>().type, PrimitiveType::Kind::Int64);
	}

	TEST_F(TypeResolverTest, VariabeleDeclaration_BasicType)
	{
		auto expr = BinaryNode::create(LiteralNode::create(Value{ static_cast<i64>(10) }),
		                               LiteralNode::create(Value{ static_cast<i32>(-5) }),
		                               ASTNodeOperator::Subtract);
		{
			const auto* as_binary = dynamic_cast<BinaryNode*>(expr.get());
			ASSERT_TRUE(as_binary);
			ASSERT_TRUE(as_binary->lhs->type.is<PrimitiveType>());
			ASSERT_TRUE(as_binary->rhs->type.is<PrimitiveType>());
			EXPECT_EQ(as_binary->lhs->type.as<PrimitiveType>().type, PrimitiveType::Kind::Unknown);
			EXPECT_EQ(as_binary->rhs->type.as<PrimitiveType>().type, PrimitiveType::Kind::Unknown);
			EXPECT_EQ(as_binary->type, PrimitiveType::Kind::Unknown);
		}

		auto variable_declaration = VariableDeclarationNode::create("my_variable", "i32", std::move(expr), false);
		ASSERT_TRUE(variable_declaration->type.is<PrimitiveType>());
		EXPECT_EQ(variable_declaration->type.as<PrimitiveType>().type, PrimitiveType::Kind::Unknown);

		resolve(variable_declaration.get());

		{
			const auto* as_variable_declaration = dynamic_cast<VariableDeclarationNode*>(variable_declaration.get());
			ASSERT_TRUE(as_variable_declaration);
			ASSERT_TRUE(as_variable_declaration->type.is<PrimitiveType>());
			EXPECT_EQ(as_variable_declaration->type.as<PrimitiveType>().type, PrimitiveType::Kind::Int32);

			const auto* as_binary = dynamic_cast<BinaryNode*>(as_variable_declaration->expr.get());
			ASSERT_TRUE(as_binary);
			ASSERT_TRUE(as_binary->lhs->type.is<PrimitiveType>());
			ASSERT_TRUE(as_binary->rhs->type.is<PrimitiveType>());
			EXPECT_EQ(as_binary->lhs->type.as<PrimitiveType>().type, PrimitiveType::Kind::Int64);
			EXPECT_EQ(as_binary->rhs->type.as<PrimitiveType>().type, PrimitiveType::Kind::Int64);
			EXPECT_EQ(as_binary->type, PrimitiveType::Kind::Int64);
		}
	}
}  // namespace soul::ast::visitors::ut
