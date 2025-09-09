#include <gtest/gtest.h>

#include "ast/nodes/error.h"
#include "ast/nodes/module.h"
#include "ast/nodes/struct_declaration.h"
#include "ast/nodes/variable_declaration.h"
#include "ast/visitors/stringify.h"
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
		// Prepare the types...
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
		auto expected_module = ModuleNode::create("discovery_module", std::move(module_statements));

		StringifyVisitor stringify_expected_before{};
		stringify_expected_before.accept(expected_module.get());

		TypeDiscovererVisitor type_discoverer{};
		type_discoverer.accept(expected_module.get());

		StringifyVisitor stringify_expected_after{};
		stringify_expected_after.accept(expected_module.get());

		// ...and verify the results.
		EXPECT_EQ(stringify_expected_before.string(), stringify_expected_after.string())
			<< "TypeDiscovererVisitor should not modify the original AST";

		auto first_struct_type  = Type{ StructType{
			 { PrimitiveType::Kind::Int32, PrimitiveType::Kind::Float64, PrimitiveType::Kind::String } } };
		auto second_struct_type = Type{ StructType{ { first_struct_type, PrimitiveType::Kind::Boolean } } };

		auto k_expected_types               = TypeDiscovererVisitor::basic_types();
		k_expected_types["first_struct"sv]  = first_struct_type;
		k_expected_types["second_struct"sv] = second_struct_type;
		ASSERT_EQ(k_expected_types, type_discoverer.discovered_types());
	}

	TEST_F(TypeDiscovererTest, RedefinitionOfType)
	{
		static const auto k_struct_name = "first_struct";

		// Prepare the types...
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

		// ...discover them...
		StringifyVisitor stringify_expected_before{};
		stringify_expected_before.accept(expected_module.get());

		TypeDiscovererVisitor type_discoverer{};
		type_discoverer.accept(expected_module.get());

		StringifyVisitor stringify_expected_after{};
		stringify_expected_after.accept(expected_module.get());

		// ...and verify the results.
		EXPECT_EQ(stringify_expected_before.string(), stringify_expected_after.string())
			<< "TypeDiscovererVisitor should not modify the original AST";

		auto k_expected_types              = TypeDiscovererVisitor::basic_types();
		k_expected_types["first_struct"sv] = Type{ StructType{
			{ PrimitiveType::Kind::Int32, PrimitiveType::Kind::Float64, PrimitiveType::Kind::String } } };
		EXPECT_EQ(k_expected_types, type_discoverer.discovered_types());

		const auto&      result_module = type_discoverer.cloned();
		StringifyVisitor stringify_result{};
		stringify_result.accept(result_module.get());

		const auto* as_result_module = dynamic_cast<ModuleNode*>(result_module.get());
		ASSERT_TRUE(as_result_module);
		ASSERT_EQ(as_result_module->statements.size(), 2);

		const auto* as_struct_declaration = dynamic_cast<StructDeclarationNode*>(as_result_module->statements[0].get());
		ASSERT_TRUE(as_struct_declaration);
		ASSERT_EQ(as_struct_declaration->parameters.size(), 3);

		const auto* first_parameter
			= dynamic_cast<VariableDeclarationNode*>(as_struct_declaration->parameters[0].get());
		ASSERT_TRUE(first_parameter);
		EXPECT_EQ(first_parameter->name, "my_int");
		EXPECT_EQ(first_parameter->type_identifier, "i32");
		EXPECT_FALSE(first_parameter->expr);
		EXPECT_FALSE(first_parameter->is_mutable);

		const auto* second_parameter
			= dynamic_cast<VariableDeclarationNode*>(as_struct_declaration->parameters[1].get());
		ASSERT_TRUE(second_parameter);
		EXPECT_EQ(second_parameter->name, "my_float");
		EXPECT_EQ(second_parameter->type_identifier, "f64");
		EXPECT_FALSE(second_parameter->expr);
		EXPECT_FALSE(second_parameter->is_mutable);

		const auto* third_parameter
			= dynamic_cast<VariableDeclarationNode*>(as_struct_declaration->parameters[2].get());
		ASSERT_TRUE(third_parameter);
		EXPECT_EQ(third_parameter->name, "my_string");
		EXPECT_EQ(third_parameter->type_identifier, "str");
		EXPECT_FALSE(third_parameter->expr);
		EXPECT_FALSE(third_parameter->is_mutable);

		const auto* as_error_node = dynamic_cast<ErrorNode*>(as_result_module->statements[1].get());
		ASSERT_TRUE(as_error_node);
		EXPECT_EQ(as_error_node->message, std::format("redefinition of type '{}'", k_struct_name));
	}

	TEST_F(TypeDiscovererTest, TypeNotRegistered)
	{
		// Prepare the types...
		auto invalid_type_struct_members = ASTNode::Dependencies{};
		invalid_type_struct_members.reserve(3);
		invalid_type_struct_members.emplace_back(VariableDeclarationNode::create("my_int", "i32", nullptr, false));
		invalid_type_struct_members.emplace_back(
			VariableDeclarationNode::create("non_existing", "non_existing_type", nullptr, false));
		invalid_type_struct_members.emplace_back(VariableDeclarationNode::create("my_float", "f64", nullptr, false));
		auto invalid_type_struct = StructDeclarationNode::create("my_struct", std::move(invalid_type_struct_members));

		auto module_statements = ASTNode::Dependencies{};
		module_statements.emplace_back(std::move(invalid_type_struct));
		auto expected_module = ModuleNode::create("discovery_module", std::move(module_statements));

		// ...discover them...
		StringifyVisitor stringify_expected_before{};
		stringify_expected_before.accept(expected_module.get());

		TypeDiscovererVisitor type_discoverer{};
		type_discoverer.accept(expected_module.get());

		StringifyVisitor stringify_expected_after{};
		stringify_expected_after.accept(expected_module.get());

		// ...and verify the results.
		EXPECT_EQ(stringify_expected_before.string(), stringify_expected_after.string())
			<< "TypeDiscovererVisitor should not modify the original AST";

		const auto& result_module    = type_discoverer.cloned();
		const auto* as_result_module = dynamic_cast<ModuleNode*>(result_module.get());
		ASSERT_TRUE(as_result_module);
		ASSERT_EQ(as_result_module->statements.size(), 1);

		const auto* as_struct_declaration = dynamic_cast<StructDeclarationNode*>(as_result_module->statements[0].get());
		ASSERT_TRUE(as_struct_declaration);
		ASSERT_EQ(as_struct_declaration->parameters.size(), 3);

		const auto* first_parameter
			= dynamic_cast<VariableDeclarationNode*>(as_struct_declaration->parameters[0].get());
		ASSERT_TRUE(first_parameter);
		EXPECT_EQ(first_parameter->name, "my_int");
		EXPECT_EQ(first_parameter->type_identifier, "i32");
		EXPECT_FALSE(first_parameter->expr);
		EXPECT_FALSE(first_parameter->is_mutable);

		const auto* second_parameter = dynamic_cast<ErrorNode*>(as_struct_declaration->parameters[1].get());
		ASSERT_TRUE(second_parameter);
		EXPECT_EQ(second_parameter->message, "cannot resolve type 'non_existing_type', because no such type exists");

		const auto* third_parameter
			= dynamic_cast<VariableDeclarationNode*>(as_struct_declaration->parameters[2].get());
		ASSERT_TRUE(third_parameter);
		EXPECT_EQ(third_parameter->name, "my_float");
		EXPECT_EQ(third_parameter->type_identifier, "f64");
		EXPECT_FALSE(third_parameter->expr);
		EXPECT_FALSE(third_parameter->is_mutable);
	}
}  // namespace soul::ast::visitors::ut
