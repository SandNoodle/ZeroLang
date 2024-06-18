#include <gtest/gtest.h>

#include "lexer/lexer.h"

namespace soul::ut
{
	class LexerTest : public ::testing::Test
	{
		protected:
			soul::lexer_t _lexer;

			static bool has_eof_token(const std::vector<token_t>& tokens)
			{
				if (tokens.empty())
					return false;

				const auto& expected_token = token_t(token_type_t::token_eof);
				const auto& actual_token = tokens.back();
				return expected_token == actual_token;
			}
	};

	TEST_F(LexerTest, EmptyString)
	{
		const std::string_view empty_string = "\0";
		const auto result_tokens = _lexer.tokenize(empty_string);
		ASSERT_EQ(result_tokens.size(), 1);
		ASSERT_EQ(result_tokens[0].type(), token_type_t::token_eof);
		ASSERT_TRUE(result_tokens[0].no_value());
		ASSERT_TRUE(_lexer.diagnostics().empty());
	}

	TEST_F(LexerTest, Literals_Identifiers)
	{
		const std::string string = "my_identifier invalid_variable this_should_work";
		const std::vector<std::string> expected_values = {
			"my_identifier",
			"invalid_variable",
			"this_should_work",
		};
		const auto result_tokens = _lexer.tokenize(string);

		ASSERT_EQ(expected_values.size(), result_tokens.size() - 1);
		for (size_t index = 0; index < expected_values.size(); ++index)
		{
			const auto& expected_token = token_t(token_type_t::token_literal_identifier, expected_values[index]);
			const auto& result_token = result_tokens[index];
			ASSERT_EQ(expected_token, result_token);
			ASSERT_TRUE(result_token.has<std::string>());
		}

		ASSERT_TRUE(has_eof_token(result_tokens));
		ASSERT_TRUE(_lexer.diagnostics().empty());
	}

	TEST_F(LexerTest, Literals_Keywords)
	{
		const std::string_view string = "native let mut if else for while continue break return fn struct true false";
		const std::vector expected_tokens = {
			token_t(token_type_t::token_native),
			token_t(token_type_t::token_let),
			token_t(token_type_t::token_mut),
			token_t(token_type_t::token_if),
			token_t(token_type_t::token_else),
			token_t(token_type_t::token_for),
			token_t(token_type_t::token_while),
			token_t(token_type_t::token_continue),
			token_t(token_type_t::token_break),
			token_t(token_type_t::token_return),
			token_t(token_type_t::token_fn),
			token_t(token_type_t::token_struct),
			token_t(token_type_t::token_true),
			token_t(token_type_t::token_false),
		};
		const auto result_tokens = _lexer.tokenize(string);

		ASSERT_EQ(expected_tokens.size(), result_tokens.size() - 1);
		for (size_t index = 0; index < expected_tokens.size(); ++index)
		{
			const auto& expected_token = expected_tokens[index];
			const auto& result_token = result_tokens[index];
			ASSERT_EQ(expected_token, result_token);
			EXPECT_TRUE(result_token.no_value()) << "Expected no value, but got: " << std::string(result_token);
		}

		ASSERT_TRUE(has_eof_token(result_tokens));
		ASSERT_TRUE(_lexer.diagnostics().empty());
	}

	TEST_F(LexerTest, SpecialCharacters)
	{
		const std::string_view string = "; ? % ^ . , ( ) { } [ ] : :: = == ! != > >= < <= + += ++ - -= -- * *= / /= & && | ||";
		const std::vector<token_type_t> expected_types = {
			// Single character tokens
			token_type_t::token_semicolon, token_type_t::token_question_mark,
			token_type_t::token_percent, token_type_t::token_caret,
			token_type_t::token_dot, token_type_t::token_comma,
			token_type_t::token_paren_left, token_type_t::token_paren_right,
			token_type_t::token_brace_left, token_type_t::token_brace_right,
			token_type_t::token_bracket_left, token_type_t::token_bracket_right,

			// One or two character tokens
			token_type_t::token_colon, token_type_t::token_double_colon,
			token_type_t::token_equal, token_type_t::token_double_equal,
			token_type_t::token_bang, token_type_t::token_bang_equal,
			token_type_t::token_greater, token_type_t::token_greater_equal,
			token_type_t::token_less, token_type_t::token_less_equal,
			token_type_t::token_plus, token_type_t::token_plus_equal, token_type_t::token_double_plus,
			token_type_t::token_minus, token_type_t::token_minus_equal, token_type_t::token_double_minus,
			token_type_t::token_star, token_type_t::token_star_equal,
			token_type_t::token_slash, token_type_t::token_slash_equal,
			token_type_t::token_ampersand, token_type_t::token_double_ampersand,
			token_type_t::token_pipe, token_type_t::token_double_pipe,
		};
		const auto result_tokens = _lexer.tokenize(string);

		ASSERT_EQ(expected_types.size(), result_tokens.size() - 1);
		for (size_t index = 0; index < expected_types.size(); ++index)
		{
			const auto& expected_token = token_t(expected_types[index]);
			const auto& result_token = result_tokens[index];
			ASSERT_EQ(expected_token, result_token);
			EXPECT_TRUE(result_tokens[index].no_value()) << "Expected no value, but got: " << std::string(result_tokens[index]);
		}

		ASSERT_TRUE(has_eof_token(result_tokens));
		ASSERT_TRUE(_lexer.diagnostics().empty());
	}

	TEST_F(LexerTest, Literals_Numbers_Integers)
	{
		const std::string_view string = "0 54 1024 4098 -8192 1000000000000";
		const std::vector<int64_t> expected_values = {
			0, 54, 1024, 4098, -8192, 1000000000000,
		};
		const auto result_tokens = _lexer.tokenize(string);

		ASSERT_EQ(expected_values.size(), result_tokens.size() - 1);
		for (size_t index = 0; index < expected_values.size(); ++index)
		{
			const auto& expected_token = token_t(token_type_t::token_literal_integer, expected_values[index]);
			const auto& result_token = result_tokens[index];
			ASSERT_EQ(expected_token, result_token);
			ASSERT_TRUE(result_tokens[index].has<int64_t>()) << "Expected integer value, but got: " << std::string(result_tokens[index]);
			EXPECT_EQ(expected_token.get<int64_t>(), expected_values[index]);
		}

		ASSERT_TRUE(has_eof_token(result_tokens));
		ASSERT_TRUE(_lexer.diagnostics().empty());
	}

	TEST_F(LexerTest, Literals_Numbers_Integers_OutOfRange)
	{
		const std::string_view string = "100000000000000000000"; // Number sufficient to be Out of Range.
		const auto result_tokens = _lexer.tokenize(string);

		ASSERT_EQ(result_tokens.size(), 1);
		ASSERT_TRUE(has_eof_token(result_tokens));

		const auto& diagnostics = _lexer.diagnostics();
		ASSERT_EQ(diagnostics.size(), 1);
		ASSERT_EQ(diagnostics[0].code(), diagnostic_code_t::error_lexer_value_out_of_range);
	}

	TEST_F(LexerTest, Literals_Numbers_FloatingPoint)
	{
		const std::string_view string = "0.0 -0.01 5.47 7.52 4098.0 -8192.32 1000000000000.0";
		const std::vector<double> expected_values = {
			0.0, -0.01, 5.47, 7.52, 4098.0, -8192.32, 1000000000000.0
		};
		const auto result_tokens = _lexer.tokenize(string);

		ASSERT_EQ(expected_values.size(), result_tokens.size() - 1);
		for (size_t index = 0; index < expected_values.size(); ++index)
		{
			const auto& expected_token = token_t(token_type_t::token_literal_float, expected_values[index]);
			const auto& result_token = result_tokens[index];
			ASSERT_EQ(expected_token, result_token);
			ASSERT_TRUE(result_tokens[index].has<double>()) << "Expected floating point value, but got: " << std::string(result_tokens[index]);
			EXPECT_EQ(expected_token.get<double>(), expected_values[index]);
		}

		ASSERT_TRUE(has_eof_token(result_tokens));
		ASSERT_TRUE(_lexer.diagnostics().empty());
	}

	TEST_F(LexerTest, Literals_Numbers_Mixed)
	{
		const std::string_view string = "0 0.0 -4.14 5 8.72 20 -40 -1024.0 0.02";
		const std::vector expected_tokens = {
			token_t(token_type_t::token_literal_integer, static_cast<int64_t>(0)),
			token_t(token_type_t::token_literal_float, static_cast<double>(0.0)),
			token_t(token_type_t::token_literal_float, static_cast<double>(-4.14)),
			token_t(token_type_t::token_literal_integer, static_cast<int64_t>(5)),
			token_t(token_type_t::token_literal_float, static_cast<double>(8.72)),
			token_t(token_type_t::token_literal_integer, static_cast<int64_t>(20)),
			token_t(token_type_t::token_literal_integer, static_cast<int64_t>(-40)),
			token_t(token_type_t::token_literal_float, static_cast<double>(-1024.0)),
			token_t(token_type_t::token_literal_float, static_cast<double>(0.02)),
		};
		const auto result_tokens = _lexer.tokenize(string);

		ASSERT_EQ(expected_tokens.size(), result_tokens.size() - 1);
		for (size_t index = 0; index < expected_tokens.size(); ++index)
		{
			const auto& expected_token = expected_tokens[index];
			const auto& result_token = result_tokens[index];
			ASSERT_EQ(expected_token, result_token);
			if (result_token.type() == token_type_t::token_literal_float)
			{
				ASSERT_TRUE(result_token.has<double>() == expected_token.has<double>());
				EXPECT_EQ(expected_token.get<double>(), result_token.get<double>());
			}
			else
			{
				ASSERT_TRUE(result_token.has<int64_t>() == expected_token.has<int64_t>());
				EXPECT_EQ(expected_token.get<int64_t>(), result_token.get<int64_t>());
			}
		}

		ASSERT_TRUE(has_eof_token(result_tokens));
		ASSERT_TRUE(_lexer.diagnostics().empty());
	}

	TEST_F(LexerTest, Literals_Strings)
	{
		const std::string_view string = R"("my_value""no space after previous" "520")";
		const std::vector<std::string> expected_values = {
			"my_value", "no space after previous", "520"
		};
		const auto result_tokens = _lexer.tokenize(string);

		ASSERT_EQ(expected_values.size(), result_tokens.size() - 1);
		for (size_t index = 0; index < expected_values.size(); ++index)
		{
			const auto expected_token = token_t(token_type_t::token_literal_string, expected_values[index]);
			const auto& result_token = result_tokens[index];
			ASSERT_EQ(expected_token, result_token);
			ASSERT_TRUE(result_tokens[index].has<std::string>()) << "Expected string value, but got: " << std::string(result_tokens[index]);
			EXPECT_EQ(expected_token.get<std::string>(), expected_values[index]);
		}

		ASSERT_TRUE(has_eof_token(result_tokens));
		ASSERT_TRUE(_lexer.diagnostics().empty());
	}

	TEST_F(LexerTest, Literals_Strings_UnterminatedString)
	{
		const std::string_view string = "\"this is an unterminated string, how sad :c";
		const auto result_tokens = _lexer.tokenize(string);

		ASSERT_EQ(result_tokens.size(), 1);
		ASSERT_TRUE(has_eof_token(result_tokens));

		const auto& diagnostics = _lexer.diagnostics();
		ASSERT_EQ(diagnostics.size(), 1);
		ASSERT_EQ(diagnostics[0].code(), diagnostic_code_t::error_lexer_unterminated_string);
	}

	TEST_F(LexerTest, All)
	{
		const std::string_view string = R"(
			fn main :: void
			{
				let my_variable : str = "my_string";
				return 0;
			}
		)";
		const std::vector expected_tokens = {
			token_t(token_type_t::token_fn),
			token_t(token_type_t::token_literal_identifier, "main"),
			token_t(token_type_t::token_double_colon),
			token_t(token_type_t::token_literal_identifier, "void"),
			token_t(token_type_t::token_brace_left),
			token_t(token_type_t::token_let),
			token_t(token_type_t::token_literal_identifier, "my_variable"),
			token_t(token_type_t::token_colon),
			token_t(token_type_t::token_literal_identifier, "str"),
			token_t(token_type_t::token_equal),
			token_t(token_type_t::token_literal_string, "my_string"),
			token_t(token_type_t::token_semicolon),
			token_t(token_type_t::token_return),
			token_t(token_type_t::token_literal_integer, static_cast<int64_t>(0)),
			token_t(token_type_t::token_semicolon),
			token_t(token_type_t::token_brace_right),
		};
		const auto result_tokens = _lexer.tokenize(string);

		ASSERT_EQ(expected_tokens.size(), result_tokens.size() - 1);
		for (size_t index = 0; index < expected_tokens.size(); ++index)
		{
			const auto& expected_token = expected_tokens[index];
			const auto& result_token = result_tokens[index];
			ASSERT_EQ(expected_token, result_token);
		}

		ASSERT_TRUE(has_eof_token(result_tokens));
		ASSERT_TRUE(_lexer.diagnostics().empty());
	}
}  // soul::ut
