#include <gtest/gtest.h>

#include "ast/ast.h"
#include "ast/nodes/variable_declaration.h"
#include "lexer/lexer.h"
#include "parser/parser.h"

namespace soul::ut
{
	class ParserTest : public ::testing::Test
	{
		protected:
		soul::Lexer  _lexer;
		soul::Parser _parser;
	};

	TEST_F(ParserTest, VariableDeclaration)
	{
		const std::string_view      script      = "let my_cheeseburger : int = 32";
		auto                        tokens      = _lexer.tokenize(script);
		const ASTNode::Dependency   result_tree = _parser.parse(tokens);
		ASSERT_TRUE(result_tree);

		ASTNode::Dependency expected_tree = nullptr;
		ASSERT_EQ(expected_tree, result_tree);
	}

	TEST_F(ParserTest, DISABLED_BlockStatement_Empty)
	{
		const std::string_view      script      = "{}";
		auto                        tokens      = _lexer.tokenize(script);
		const ASTNode::Dependency   result_tree = _parser.parse(tokens);
		ASSERT_TRUE(result_tree);

		std::vector<ASTNode::Dependency> statements;
		ASTNode::Dependency              expected_tree = nullptr;  // TODO
		ASSERT_EQ(expected_tree, result_tree);
	}
}  // namespace soul::ut
