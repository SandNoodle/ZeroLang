#include <gtest/gtest.h>

#include "ast/ast.h"
#include "ast/nodes/literal.h"
#include "ast/nodes/module.h"
#include "ast/nodes/variable_declaration.h"
#include "ast/visitors/stringify.h"
#include "lexer/lexer.h"
#include "parser/parser.h"

namespace soul::ut
{
	using namespace std::string_view_literals;

	struct Case
	{
		std::string case_name;
		std::string case_path;
		std::string input;
		std::string expected;
	};

	class ParserTest : public ::testing::TestWithParam<Case>
	{
		protected:
		soul::Lexer  _lexer;
		soul::Parser _parser;
	};

	TEST_P(ParserTest, Foo)
	{
		const auto& param = GetParam();

		const auto tokens      = _lexer.tokenize(param.input);
		const auto result_tree = _parser.parse(tokens);

		StringifyVisitor stringify;
		stringify.accept(result_tree.get());

		ASSERT_EQ(stringify.string(), param.expected);
	}
}  // namespace soul::ut
