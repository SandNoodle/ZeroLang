#include <gtest/gtest.h>

#include "ast/nodes/error.h"
#include "ast/nodes/module.h"
#include "ast/nodes/struct_declaration.h"
#include "ast/nodes/variable_declaration.h"
#include "ast/visitors/type_discoverer.h"

#include <format>

namespace soul::ast::visitors::ut
{
	using namespace types;
	using namespace ast::nodes;
	using namespace std::string_view_literals;

	class TypeDiscovererTest : public ::testing::Test
	{
	};

	TEST_F(TypeDiscovererTest, StructUsedByOtherStruct)
	{
		auto first_struct_members = ASTNode::Dependencies{};
		first_struct_members.reserve(3);
		first_struct_members.emplace_back(VariableDeclarationNode::create("my_int", "i32", nullptr, false));
		first_struct_members.emplace_back(VariableDeclarationNode::create("my_float", "f64", nullptr, false));
		first_struct_members.emplace_back(VariableDeclarationNode::create("my_string", "str", nullptr, false));
		auto first_struct = StructDeclarationNode::create("first_struct", std::move(first_struct_members));

		auto second_struct_members = ASTNode::Dependencies{};
		second_struct_members.reserve(2);
		second_struct_members.emplace_back(
			VariableDeclarationNode::create("my_struct", "first_struct", nullptr, false));
		second_struct_members.emplace_back(VariableDeclarationNode::create("my_bool", "bool", nullptr, false));
		auto second_struct = StructDeclarationNode::create("second_struct", std::move(second_struct_members));

		auto module_statements = ASTNode::Dependencies{};
		module_statements.emplace_back(std::move(first_struct));
		module_statements.emplace_back(std::move(second_struct));

		auto module = ModuleNode::create("discovery_module", std::move(module_statements));

		TypeDiscovererVisitor type_discoverer{};
		type_discoverer.accept(module.get());

		auto first_struct_type  = Type{ StructType{
			 { PrimitiveType::Kind::Int32, PrimitiveType::Kind::Float64, PrimitiveType::Kind::String } } };
		auto second_struct_type = Type{ StructType{ { first_struct_type, PrimitiveType::Kind::Boolean } } };

		auto k_expected_types               = TypeDiscovererVisitor::basic_types();
		k_expected_types["first_struct"sv]  = first_struct_type;
		k_expected_types["second_struct"sv] = second_struct_type;
		ASSERT_EQ(k_expected_types, type_discoverer.get());
	}

	TEST_F(TypeDiscovererTest, RedeclarationOfType)
	{
		static const auto k_struct_name = "first_struct";

		auto first_struct_members = ASTNode::Dependencies{};
		first_struct_members.reserve(3);
		first_struct_members.emplace_back(VariableDeclarationNode::create("my_int", "i32", nullptr, false));
		first_struct_members.emplace_back(VariableDeclarationNode::create("my_float", "f64", nullptr, false));
		first_struct_members.emplace_back(VariableDeclarationNode::create("my_string", "str", nullptr, false));
		auto first_struct = StructDeclarationNode::create(k_struct_name, std::move(first_struct_members));

		auto second_struct_members = ASTNode::Dependencies{};
		second_struct_members.reserve(2);
		second_struct_members.emplace_back(VariableDeclarationNode::create("my_string", "str", nullptr, false));
		second_struct_members.emplace_back(VariableDeclarationNode::create("my_bool", "bool", nullptr, false));
		auto first_struct_redeclared = StructDeclarationNode::create(k_struct_name, std::move(second_struct_members));

		auto module_statements = ASTNode::Dependencies{};
		module_statements.emplace_back(std::move(first_struct));
		module_statements.emplace_back(std::move(first_struct_redeclared));

		auto expected_module = ModuleNode::create("discovery_module", std::move(module_statements));

		TypeDiscovererVisitor type_discoverer{};
		type_discoverer.accept(expected_module.get());

		auto first_struct_type = Type{ StructType{
			{ PrimitiveType::Kind::Int32, PrimitiveType::Kind::Float64, PrimitiveType::Kind::String } } };

		auto k_expected_types              = TypeDiscovererVisitor::basic_types();
		k_expected_types["first_struct"sv] = first_struct_type;
		EXPECT_EQ(k_expected_types, type_discoverer.get());

		const auto& result_module    = type_discoverer.cloned();
		const auto* as_result_module = dynamic_cast<ModuleNode*>(result_module.get());
		ASSERT_TRUE(as_result_module);
		ASSERT_EQ(as_result_module->statements.size(), 2);

		const auto* as_struct_declaration = dynamic_cast<StructDeclarationNode*>(as_result_module->statements[0].get());
		ASSERT_TRUE(as_struct_declaration);

		const auto* as_error_node = dynamic_cast<ErrorNode*>(as_result_module->statements[1].get());
		ASSERT_TRUE(as_error_node);
		ASSERT_EQ(as_error_node->message, std::format("redefinition of type '{}'", k_struct_name));
	}

}  // namespace soul::ast::visitors::ut
