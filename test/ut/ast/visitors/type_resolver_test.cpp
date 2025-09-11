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
