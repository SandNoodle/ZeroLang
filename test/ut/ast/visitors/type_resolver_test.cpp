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
		ASTNode::Dependency resolve(ASTNode::Reference root)
		{
			TypeDiscovererVisitor type_discoverer_visitor{};
			type_discoverer_visitor.accept(root);

			auto type_discoverer_root = type_discoverer_visitor.cloned();

			// TODO: Check if it does not contain errors.

			TypeResolverVisitor type_resolver_visitor{ type_discoverer_visitor.discovered_types() };
			type_resolver_visitor.accept(type_discoverer_root.get());

			return type_resolver_visitor.cloned();
		}
	};

	TEST_F(TypeResolverTest, Cast_BasicType)
	{
		auto cast_node = CastNode::create(LiteralNode::create(Value{ 128L }, LiteralNode::LiteralType::Int64), "i32");
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
		EXPECT_EQ(as_literal->literal_type, LiteralNode::LiteralType::Int64);
		EXPECT_EQ(as_literal->type, PrimitiveType::Kind::Int64);
	}

	TEST_F(TypeResolverTest, Cast_Impossible)
	{
		auto cast_node = CastNode::create(LiteralNode::create(Value{ 128L }, LiteralNode::LiteralType::Int64), "chr");
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
			"index", "i32", LiteralNode::create(Value{ 0 }, LiteralNode::LiteralType::Int32), true);

		auto condition = BinaryNode::create(LiteralNode::create(Value{ "index" }, LiteralNode::LiteralType::Identifier),
		                                    LiteralNode::create(Value{ 10 }, LiteralNode::LiteralType::Int32),
		                                    ASTNode::Operator::Less);

		auto update = UnaryNode::create(LiteralNode::create(Value{ "index" }, LiteralNode::LiteralType::Identifier),
		                                ASTNode::Operator::Increment);

		auto for_loop_statements = ASTNode::Dependencies{};
		for_loop_statements.emplace_back(LiteralNode::create(Value{ "my_string" }, LiteralNode::LiteralType::String));

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
		ASSERT_TRUE(as_initialization->expr);
		{
			const auto* as_value = dynamic_cast<LiteralNode*>(as_initialization->expr.get());
			ASSERT_TRUE(as_value);
			EXPECT_EQ(as_value->type, PrimitiveType::Kind::Int32);
			EXPECT_EQ(as_value->value, Value{ 0 });
			EXPECT_EQ(as_value->literal_type, LiteralNode::LiteralType::Int32);
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
			EXPECT_EQ(as_lhs->literal_type, LiteralNode::LiteralType::Identifier);

			const auto* as_rhs = dynamic_cast<LiteralNode*>(as_condition->rhs.get());
			ASSERT_TRUE(as_rhs);
			EXPECT_EQ(as_rhs->type, PrimitiveType::Kind::Int32);
			EXPECT_EQ(as_rhs->value, Value{ 10 });
			EXPECT_EQ(as_rhs->literal_type, LiteralNode::LiteralType::Int32);
		}

		const auto* as_update = dynamic_cast<UnaryNode*>(as_for_loop->update.get());
		ASSERT_TRUE(as_update);
		EXPECT_EQ(as_update->type, PrimitiveType::Kind::Int32);
		{
			EXPECT_EQ(as_update->op, ASTNode::Operator::Increment);

			const auto* as_value = dynamic_cast<LiteralNode*>(as_update->expr.get());
			ASSERT_TRUE(as_value);
			EXPECT_EQ(as_value->type, PrimitiveType::Kind::Int32);
			EXPECT_EQ(as_value->value, Value{ "index" });
			EXPECT_EQ(as_value->literal_type, LiteralNode::LiteralType::Identifier);
		}

		const auto* as_statements = dynamic_cast<BlockNode*>(as_for_loop->statements.get());
		ASSERT_TRUE(as_statements);
		ASSERT_EQ(as_statements->statements.size(), 1);
	}

	TEST_F(TypeResolverTest, ForLoop_ConditionNotBool)
	{
		auto condition = BinaryNode::create(LiteralNode::create(Value{ "index" }, LiteralNode::LiteralType::Identifier),
		                                    LiteralNode::create(Value{ 10 }, LiteralNode::LiteralType::Int32),
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
		EXPECT_FALSE(as_variable_declaration->expr);
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
		EXPECT_FALSE(as_variable_declaration->expr);
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
		EXPECT_FALSE(as_inner_variable->expr);
		EXPECT_FALSE(as_inner_variable->is_mutable);
		EXPECT_EQ(as_inner_variable->type, PrimitiveType::Kind::Int32);

		const auto* as_outer_variable = dynamic_cast<VariableDeclarationNode*>(as_module->statements[1].get());
		ASSERT_TRUE(as_outer_variable);
		EXPECT_EQ(as_outer_variable->name, k_variable_name);
		EXPECT_EQ(as_outer_variable->type_identifier, "f32");
		EXPECT_FALSE(as_outer_variable->expr);
		EXPECT_TRUE(as_outer_variable->is_mutable);
		EXPECT_EQ(as_outer_variable->type, PrimitiveType::Kind::Float32);
	}
}  // namespace soul::ast::visitors::ut
