#include <gtest/gtest.h>

#include "backend/codegen/wasm/compiler.h"
#include "lexer/lexer.h"
#include "parser/parser.h"

namespace soul::backend::codegen::wasm::ut
{
	using namespace parser;
	using namespace lexer;

	class WASMCompilerTest : public ::testing::Test
	{
	};

	TEST_F(WASMCompilerTest, EmptyModule) {
		auto result = WASMCompiler::compile(nullptr);
		ASSERT_TRUE(result.empty());
	}

	TEST_F(WASMCompilerTest, SimpleVariableDeclaration) {
		auto tokens = Lexer::tokenize("let my_variable : u32 = 0;");
		auto root = Parser::parse(tokens);
		auto result = WASMCompiler::compile(root.get());
		// TODO: Decode headers, etc.
	}

}  // namespace soul::backend::codegen::wasm::ut
