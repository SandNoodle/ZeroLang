#include <gtest/gtest.h>

#include "lexer/lexer.h"

namespace soul::ut
{
	class LexerTest : public ::testing::Test
	{
		protected:
		soul::Lexer _lexer;

		static bool has_eof_token(const std::vector<Token>& tokens)
		{
			if (tokens.empty()) return false;

			const auto& expected_token = Token(TokenType::TOKEN_EOF);
			const auto& actual_token   = tokens.back();
			return expected_token == actual_token;
		}
	};

	TEST_F(LexerTest, EmptyString)
	{
		const std::string_view empty_string  = "\0";
		const auto             result_tokens = _lexer.tokenize(empty_string);
		ASSERT_EQ(result_tokens.size(), 1);
		ASSERT_EQ(result_tokens[0].type(), TokenType::TOKEN_EOF);
		ASSERT_TRUE(result_tokens[0].no_value());
		ASSERT_TRUE(_lexer.diagnostics().empty());
	}

	TEST_F(LexerTest, Literals_Identifiers)
	{
		const std::string              string          = "my_identifier invalid_variable this_should_work";
		const std::vector<std::string> expected_values = {
			"my_identifier",
			"invalid_variable",
			"this_should_work",
		};
		const auto result_tokens = _lexer.tokenize(string);

		ASSERT_EQ(expected_values.size(), result_tokens.size() - 1);
		for (size_t index = 0; index < expected_values.size(); ++index) {
			const auto& expected_token = Token(TokenType::TOKEN_LITERAL_IDENTIFIER, expected_values[index]);
			const auto& result_token   = result_tokens[index];
			ASSERT_EQ(expected_token, result_token);
			ASSERT_TRUE(result_token.has<std::string>());
		}

		ASSERT_TRUE(has_eof_token(result_tokens));
		ASSERT_TRUE(_lexer.diagnostics().empty());
	}

	TEST_F(LexerTest, Literals_Keywords)
	{
		const std::string_view string = "native let mut if else for while continue break return fn struct true false";
		const std::vector      expected_tokens = {
            Token(TokenType::TOKEN_NATIVE), Token(TokenType::TOKEN_LET),      Token(TokenType::TOKEN_MUT),
            Token(TokenType::TOKEN_IF),     Token(TokenType::TOKEN_ELSE),     Token(TokenType::TOKEN_FOR),
            Token(TokenType::TOKEN_WHILE),  Token(TokenType::TOKEN_CONTINUE), Token(TokenType::TOKEN_BREAK),
            Token(TokenType::TOKEN_RETURN), Token(TokenType::TOKEN_FN),       Token(TokenType::TOKEN_STRUCT),
            Token(TokenType::TOKEN_TRUE),   Token(TokenType::TOKEN_FALSE),
		};
		const auto result_tokens = _lexer.tokenize(string);

		ASSERT_EQ(expected_tokens.size(), result_tokens.size() - 1);
		for (size_t index = 0; index < expected_tokens.size(); ++index) {
			const auto& expected_token = expected_tokens[index];
			const auto& result_token   = result_tokens[index];
			ASSERT_EQ(expected_token, result_token);
			EXPECT_TRUE(result_token.no_value()) << "Expected no value, but got: " << std::string(result_token);
		}

		ASSERT_TRUE(has_eof_token(result_tokens));
		ASSERT_TRUE(_lexer.diagnostics().empty());
	}

	TEST_F(LexerTest, SpecialCharacters)
	{
		const std::string_view string
			= "; ? % ^ . , ( ) { } [ ] : :: = == ! != > >= < <= + += ++ - -= -- * *= / /= & && | ||";
		const std::vector<TokenType> expected_types = {
			// Single character tokens
			TokenType::TOKEN_SEMICOLON,
			TokenType::TOKEN_QUESTION_MARK,
			TokenType::TOKEN_PERCENT,
			TokenType::TOKEN_CARET,
			TokenType::TOKEN_DOT,
			TokenType::TOKEN_COMMA,
			TokenType::TOKEN_PAREN_LEFT,
			TokenType::TOKEN_PAREN_RIGHT,
			TokenType::TOKEN_BRACE_LEFT,
			TokenType::TOKEN_BRACE_RIGHT,
			TokenType::TOKEN_BRACKET_LEFT,
			TokenType::TOKEN_BRACKET_RIGHT,

			// One or two character tokens
			TokenType::TOKEN_COLON,
			TokenType::TOKEN_DOUBLE_COLON,
			TokenType::TOKEN_EQUAL,
			TokenType::TOKEN_DOUBLE_EQUAL,
			TokenType::TOKEN_BANG,
			TokenType::TOKEN_BANG_EQUAL,
			TokenType::TOKEN_GREATER,
			TokenType::TOKEN_GREATER_EQUAL,
			TokenType::TOKEN_LESS,
			TokenType::TOKEN_LESS_EQUAL,
			TokenType::TOKEN_PLUS,
			TokenType::TOKEN_PLUS_EQUAL,
			TokenType::TOKEN_DOUBLE_PLUS,
			TokenType::TOKEN_MINUS,
			TokenType::TOKEN_MINUS_EQUAL,
			TokenType::TOKEN_DOUBLE_MINUS,
			TokenType::TOKEN_STAR,
			TokenType::TOKEN_STAR_EQUAL,
			TokenType::TOKEN_SLASH,
			TokenType::TOKEN_SLASH_EQUAL,
			TokenType::TOKEN_AMPERSAND,
			TokenType::TOKEN_DOUBLE_AMPERSAND,
			TokenType::TOKEN_PIPE,
			TokenType::TOKEN_DOUBLE_PIPE,
		};
		const auto result_tokens = _lexer.tokenize(string);

		ASSERT_EQ(expected_types.size(), result_tokens.size() - 1);
		for (size_t index = 0; index < expected_types.size(); ++index) {
			const auto& expected_token = Token(expected_types[index]);
			const auto& result_token   = result_tokens[index];
			ASSERT_EQ(expected_token, result_token);
			EXPECT_TRUE(result_tokens[index].no_value())
				<< "Expected no value, but got: " << std::string(result_tokens[index]);
		}

		ASSERT_TRUE(has_eof_token(result_tokens));
		ASSERT_TRUE(_lexer.diagnostics().empty());
	}

	TEST_F(LexerTest, Literals_Numbers_Integers)
	{
		const std::string_view     string          = "0 54 1024 4098 -8192 1000000000000";
		const std::vector<int64_t> expected_values = {
			0, 54, 1024, 4098, -8192, 1000000000000,
		};
		const auto result_tokens = _lexer.tokenize(string);

		ASSERT_EQ(expected_values.size(), result_tokens.size() - 1);
		for (size_t index = 0; index < expected_values.size(); ++index) {
			const auto& expected_token = Token(TokenType::TOKEN_LITERAL_INTEGER, expected_values[index]);
			const auto& result_token   = result_tokens[index];
			ASSERT_EQ(expected_token, result_token);
			ASSERT_TRUE(result_tokens[index].has<int64_t>())
				<< "Expected integer value, but got: " << std::string(result_tokens[index]);
			EXPECT_EQ(expected_token.get<int64_t>(), expected_values[index]);
		}

		ASSERT_TRUE(has_eof_token(result_tokens));
		ASSERT_TRUE(_lexer.diagnostics().empty());
	}

	TEST_F(LexerTest, Literals_Numbers_Integers_OutOfRange)
	{
		const std::string_view string        = "100000000000000000000";  // Number sufficient to be Out of Range.
		const auto             result_tokens = _lexer.tokenize(string);

		ASSERT_EQ(result_tokens.size(), 1);
		ASSERT_TRUE(has_eof_token(result_tokens));

		const auto& diagnostics = _lexer.diagnostics();
		ASSERT_EQ(diagnostics.size(), 1);
		ASSERT_EQ(diagnostics[0].code(), DiagnosticCode::ERROR_LEXER_VALUE_OUT_OF_RANGE);
	}

	TEST_F(LexerTest, Literals_Numbers_FloatingPoint)
	{
		const std::string_view    string          = "0.0 -0.01 5.47 7.52 4098.0 -8192.32 1000000000000.0";
		const std::vector<double> expected_values = { 0.0, -0.01, 5.47, 7.52, 4098.0, -8192.32, 1000000000000.0 };
		const auto                result_tokens   = _lexer.tokenize(string);

		ASSERT_EQ(expected_values.size(), result_tokens.size() - 1);
		for (size_t index = 0; index < expected_values.size(); ++index) {
			const auto& expected_token = Token(TokenType::TOKEN_LITERAL_FLOAT, expected_values[index]);
			const auto& result_token   = result_tokens[index];
			ASSERT_EQ(expected_token, result_token);
			ASSERT_TRUE(result_tokens[index].has<double>())
				<< "Expected floating point value, but got: " << std::string(result_tokens[index]);
			EXPECT_EQ(expected_token.get<double>(), expected_values[index]);
		}

		ASSERT_TRUE(has_eof_token(result_tokens));
		ASSERT_TRUE(_lexer.diagnostics().empty());
	}

	TEST_F(LexerTest, Literals_Numbers_Mixed)
	{
		const std::string_view string          = "0 0.0 -4.14 5 8.72 20 -40 -1024.0 0.02";
		const std::vector      expected_tokens = {
            Token(TokenType::TOKEN_LITERAL_INTEGER, static_cast<int64_t>(0)),
            Token(TokenType::TOKEN_LITERAL_FLOAT, static_cast<double>(0.0)),
            Token(TokenType::TOKEN_LITERAL_FLOAT, static_cast<double>(-4.14)),
            Token(TokenType::TOKEN_LITERAL_INTEGER, static_cast<int64_t>(5)),
            Token(TokenType::TOKEN_LITERAL_FLOAT, static_cast<double>(8.72)),
            Token(TokenType::TOKEN_LITERAL_INTEGER, static_cast<int64_t>(20)),
            Token(TokenType::TOKEN_LITERAL_INTEGER, static_cast<int64_t>(-40)),
            Token(TokenType::TOKEN_LITERAL_FLOAT, static_cast<double>(-1024.0)),
            Token(TokenType::TOKEN_LITERAL_FLOAT, static_cast<double>(0.02)),
		};
		const auto result_tokens = _lexer.tokenize(string);

		ASSERT_EQ(expected_tokens.size(), result_tokens.size() - 1);
		for (size_t index = 0; index < expected_tokens.size(); ++index) {
			const auto& expected_token = expected_tokens[index];
			const auto& result_token   = result_tokens[index];
			ASSERT_EQ(expected_token, result_token);
			if (result_token.type() == TokenType::TOKEN_LITERAL_FLOAT) {
				ASSERT_TRUE(result_token.has<double>() == expected_token.has<double>());
				EXPECT_EQ(expected_token.get<double>(), result_token.get<double>());
			} else {
				ASSERT_TRUE(result_token.has<int64_t>() == expected_token.has<int64_t>());
				EXPECT_EQ(expected_token.get<int64_t>(), result_token.get<int64_t>());
			}
		}

		ASSERT_TRUE(has_eof_token(result_tokens));
		ASSERT_TRUE(_lexer.diagnostics().empty());
	}

	TEST_F(LexerTest, Literals_Strings)
	{
		const std::string_view         string          = R"("my_value""no space after previous" "520")";
		const std::vector<std::string> expected_values = { "my_value", "no space after previous", "520" };
		const auto                     result_tokens   = _lexer.tokenize(string);

		ASSERT_EQ(expected_values.size(), result_tokens.size() - 1);
		for (size_t index = 0; index < expected_values.size(); ++index) {
			const auto  expected_token = Token(TokenType::TOKEN_LITERAL_STRING, expected_values[index]);
			const auto& result_token   = result_tokens[index];
			ASSERT_EQ(expected_token, result_token);
			ASSERT_TRUE(result_tokens[index].has<std::string>())
				<< "Expected string value, but got: " << std::string(result_tokens[index]);
			EXPECT_EQ(expected_token.get<std::string>(), expected_values[index]);
		}

		ASSERT_TRUE(has_eof_token(result_tokens));
		ASSERT_TRUE(_lexer.diagnostics().empty());
	}

	TEST_F(LexerTest, Literals_Strings_UnterminatedString)
	{
		const std::string_view string        = "\"this is an unterminated string, how sad :c";
		const auto             result_tokens = _lexer.tokenize(string);

		ASSERT_EQ(result_tokens.size(), 1);
		ASSERT_TRUE(has_eof_token(result_tokens));

		const auto& diagnostics = _lexer.diagnostics();
		ASSERT_EQ(diagnostics.size(), 1);
		ASSERT_EQ(diagnostics[0].code(), DiagnosticCode::ERROR_LEXER_UNTERMINATED_STRING);
	}

	TEST_F(LexerTest, All)
	{
		const std::string_view string          = R"(
			fn main :: void
			{
				let my_variable : str = "my_string";
				return 0;
			}
		)";
		const std::vector      expected_tokens = {
            Token(TokenType::TOKEN_FN),
            Token(TokenType::TOKEN_LITERAL_IDENTIFIER, "main"),
            Token(TokenType::TOKEN_DOUBLE_COLON),
            Token(TokenType::TOKEN_LITERAL_IDENTIFIER, "void"),
            Token(TokenType::TOKEN_BRACE_LEFT),
            Token(TokenType::TOKEN_LET),
            Token(TokenType::TOKEN_LITERAL_IDENTIFIER, "my_variable"),
            Token(TokenType::TOKEN_COLON),
            Token(TokenType::TOKEN_LITERAL_IDENTIFIER, "str"),
            Token(TokenType::TOKEN_EQUAL),
            Token(TokenType::TOKEN_LITERAL_STRING, "my_string"),
            Token(TokenType::TOKEN_SEMICOLON),
            Token(TokenType::TOKEN_RETURN),
            Token(TokenType::TOKEN_LITERAL_INTEGER, static_cast<int64_t>(0)),
            Token(TokenType::TOKEN_SEMICOLON),
            Token(TokenType::TOKEN_BRACE_RIGHT),
		};
		const auto result_tokens = _lexer.tokenize(string);

		ASSERT_EQ(expected_tokens.size(), result_tokens.size() - 1);
		for (size_t index = 0; index < expected_tokens.size(); ++index) {
			const auto& expected_token = expected_tokens[index];
			const auto& result_token   = result_tokens[index];
			ASSERT_EQ(expected_token, result_token);
		}

		ASSERT_TRUE(has_eof_token(result_tokens));
		ASSERT_TRUE(_lexer.diagnostics().empty());
	}
}  // namespace soul::ut
