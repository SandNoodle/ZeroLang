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

			auto type_resolver_root = type_resolver_visitor.cloned();

			return std::move(type_resolver_root);
		}
	};

	TEST_F(TypeResolverTest, VariableDeclarationShadowsPreviousOne)
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
		EXPECT_EQ(as_error->message,
		          std::format("variable declaration '{}' is being shadowed by a new one", k_variable_name));
	}
}  // namespace soul::ast::visitors::ut
