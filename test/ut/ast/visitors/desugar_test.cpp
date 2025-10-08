#include "ast/visitors/desugar.h"

#include <gtest/gtest.h>

#include "ast/nodes/binary.h"
#include "ast/nodes/block.h"
#include "ast/nodes/error.h"
#include "ast/nodes/for_loop.h"
#include "ast/nodes/literal.h"
#include "ast/nodes/module.h"
#include "ast/nodes/unary.h"
#include "ast/nodes/variable_declaration.h"
#include "ast/nodes/while.h"
#include "ast/visitors/error_collector.h"
#include "ast/visitors/stringify.h"
#include "ast/visitors/type_discoverer.h"
#include "ast/visitors/type_resolver.h"

#include <format>

namespace soul::ast::visitors::ut
{
	using namespace soul::ast::nodes;
	using namespace soul::types;

	class DesugarVisitorTest : public ::testing::Test
	{
		public:
		static constexpr auto k_module_name = "desugar_module";

		protected:
		ASTNode::Dependency build(ASTNode::Dependency&& root)
		{
			TypeDiscovererVisitor type_discoverer_visitor{};
			type_discoverer_visitor.accept(root.get());

			auto type_discoverer_root = type_discoverer_visitor.cloned();
			if (type_discoverer_visitor.affects()) {
				ErrorCollectorVisitor error_collector{};
				error_collector.accept(type_discoverer_root.get());
				if (!error_collector.is_valid()) {
					for (const auto& [depth, error] : error_collector.errors()) {
						std::cerr << std::format("[{}]: {}\n", depth, error->message);
					}
					return nullptr;
				}
			}

			TypeResolverVisitor type_resolver_visitor{ type_discoverer_visitor.discovered_types() };
			type_resolver_visitor.accept(type_discoverer_root.get());

			auto type_resolver_root = type_resolver_visitor.cloned();
			if (type_resolver_visitor.affects()) {
				ErrorCollectorVisitor error_collector{};
				error_collector.accept(type_resolver_root.get());
				if (!error_collector.is_valid()) {
					for (const auto& [depth, error] : error_collector.errors()) {
						std::cerr << std::format("[{}]: {}\n", depth, error->message);
					}
					return nullptr;
				}
			}

			DesugarVisitor desugar_visitor{};
			desugar_visitor.accept(type_resolver_root.get());

			return desugar_visitor.cloned();
		}

		std::pair<std::string, std::string> compare(const ASTNode::Reference expected, const ASTNode::Reference result)
		{
			StringifyVisitor stringify_expected{ StringifyVisitor::Options::PrintTypes };
			stringify_expected.accept(expected);

			StringifyVisitor stringify_result{ StringifyVisitor::Options::PrintTypes };
			stringify_result.accept(result);

			return std::make_pair(std::move(stringify_expected.string()), std::move(stringify_result.string()));
		}
	};

	TEST_F(DesugarVisitorTest, Binary)
	{
		// Get complex assignment operators...
		static constexpr std::array k_substitutable_operators = { ASTNode::Operator::AddAssign,
			                                                      ASTNode::Operator::SubAssign,
			                                                      ASTNode::Operator::MulAssign,
			                                                      ASTNode::Operator::DivAssign,
			                                                      ASTNode::Operator::ModAssign };

		auto input_module_statements = ASTNode::Dependencies{};
		input_module_statements.reserve(k_substitutable_operators.size());
		for (const auto op : k_substitutable_operators) {
			input_module_statements.emplace_back(
				BinaryNode::create(LiteralNode::create(Value{ 1 }, LiteralNode::Type::Int32),
			                       LiteralNode::create(Value{ 2 }, LiteralNode::Type::Int32),
			                       op));
		}
		// ...desugar them...
		auto result_module = build(ModuleNode::create(k_module_name, std::move(input_module_statements)));
		ASSERT_TRUE(result_module);

		// ...and verify the results.
		static constexpr std::array k_result_operators = { ASTNode::Operator::Add,
			                                               ASTNode::Operator::Sub,
			                                               ASTNode::Operator::Mul,
			                                               ASTNode::Operator::Div,
			                                               ASTNode::Operator::Mod };

		auto expected_module_statements = ASTNode::Dependencies{};
		expected_module_statements.reserve(k_result_operators.size());
		for (const auto op : k_result_operators) {
			expected_module_statements.emplace_back(
				BinaryNode::create(LiteralNode::create(Value{ 1 }, LiteralNode::Type::Int32),
			                       BinaryNode::create(LiteralNode::create(Value{ 1 }, LiteralNode::Type::Int32),
			                                          LiteralNode::create(Value{ 2 }, LiteralNode::Type::Int32),
			                                          op),
			                       ASTNode::Operator::Assign));
		}
		auto expected_module = build(ModuleNode::create(k_module_name, std::move(expected_module_statements)));
		ASSERT_TRUE(expected_module);

		auto [expected_string, result_string] = compare(expected_module.get(), result_module.get());
		ASSERT_EQ(expected_string, result_string);
	}

	TEST_F(DesugarVisitorTest, ForLoop)
	{
		// Get a ForLoop node...
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

		// ...desugar it...
		auto result_module = build(ModuleNode::create(k_module_name, std::move(input_module_statements)));
		ASSERT_TRUE(result_module);

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
		auto expected_module = build(ModuleNode::create(k_module_name, std::move(expected_module_statements)));
		ASSERT_TRUE(expected_module);

		auto [expected_string, result_string] = compare(expected_module.get(), result_module.get());
		ASSERT_EQ(expected_string, result_string);
	}
}  // namespace soul::ast::visitors::ut
