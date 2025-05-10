#include <gtest/gtest.h>

#include "lexer/lexer.h"

namespace soul::lexer::ut
{
	class LexerTest : public ::testing::Test
	{
		protected:
		Diagnostics _diagnostics;

		void TearDown() override { _diagnostics.clear(); }

		static bool has_eof_token(const std::vector<Token>& tokens)
		{
			if (tokens.empty()) return false;

			const auto& expected_token = Token(TokenType::EndOfFile);
			const auto& actual_token   = tokens.back();
			return expected_token == actual_token;
		}
	};

	TEST_F(LexerTest, EmptyString)
	{
		const std::string_view empty_string  = "\0";
		const auto             result_tokens = Lexer::tokenize(empty_string, &this->_diagnostics);
		ASSERT_EQ(result_tokens.size(), 1);
		ASSERT_EQ(result_tokens[0].type(), TokenType::EndOfFile);
		ASSERT_TRUE(result_tokens[0].no_value());
		ASSERT_TRUE(this->_diagnostics.empty());
	}

	TEST_F(LexerTest, Literals_Identifiers)
	{
		const std::string              string          = "my_identifier invalid_variable this_should_work";
		const std::vector<std::string> expected_values = {
			"my_identifier",
			"invalid_variable",
			"this_should_work",
		};
		const auto result_tokens = Lexer::tokenize(string, &this->_diagnostics);

		ASSERT_EQ(expected_values.size(), result_tokens.size() - 1);
		for (size_t index = 0; index < expected_values.size(); ++index) {
			const auto& expected_token = Token(TokenType::LiteralIdentifier, Value(expected_values[index]));
			const auto& result_token   = result_tokens[index];
			ASSERT_EQ(expected_token, result_token);
			ASSERT_TRUE(result_token.value.is<std::string>());
		}

		ASSERT_TRUE(has_eof_token(result_tokens));
		ASSERT_TRUE(this->_diagnostics.empty());
	}

	TEST_F(LexerTest, Literals_Keywords)
	{
		const std::string_view string
			= "native let mut in if else for foreach while cast continue break return fn struct true false";
		const std::vector expected_tokens = {
			Token(TokenType::KeywordNative), Token(TokenType::KeywordLet),      Token(TokenType::KeywordMut),
			Token(TokenType::KeywordIn),     Token(TokenType::KeywordIf),       Token(TokenType::KeywordElse),
			Token(TokenType::KeywordFor),    Token(TokenType::KeywordForeach),  Token(TokenType::KeywordWhile),
			Token(TokenType::KeywordCast),   Token(TokenType::KeywordContinue), Token(TokenType::KeywordBreak),
			Token(TokenType::KeywordReturn), Token(TokenType::KeywordFn),       Token(TokenType::KeywordStruct),
			Token(TokenType::KeywordTrue),   Token(TokenType::KeywordFalse),
		};
		const auto result_tokens = Lexer::tokenize(string, &this->_diagnostics);

		ASSERT_EQ(expected_tokens.size(), result_tokens.size() - 1);
		for (size_t index = 0; index < expected_tokens.size(); ++index) {
			const auto& expected_token = expected_tokens[index];
			const auto& result_token   = result_tokens[index];
			ASSERT_EQ(expected_token, result_token);
			EXPECT_TRUE(result_token.no_value()) << "Expected no value, but got: " << std::string(result_token);
		}

		ASSERT_TRUE(has_eof_token(result_tokens));
		ASSERT_TRUE(this->_diagnostics.empty());
	}

	TEST_F(LexerTest, SpecialCharacters)
	{
		const std::string_view string
			= "; ? % ^ . , ( ) { } [ ] : :: = == ! != > >= < <= + += ++ - -= -- * *= / /= & && | ||";
		const std::vector<TokenType> expected_types = {
			// Single character tokens
			TokenType::Semicolon,
			TokenType::QuestionMark,
			TokenType::Percent,
			TokenType::Caret,
			TokenType::Dot,
			TokenType::Comma,
			TokenType::ParenLeft,
			TokenType::ParenRight,
			TokenType::BraceLeft,
			TokenType::BraceRight,
			TokenType::BracketLeft,
			TokenType::BracketRight,

			// One or two character tokens
			TokenType::Colon,
			TokenType::DoubleColon,
			TokenType::Equal,
			TokenType::DoubleEqual,
			TokenType::Bang,
			TokenType::BangEqual,
			TokenType::Greater,
			TokenType::GreaterEqual,
			TokenType::Less,
			TokenType::LessEqual,
			TokenType::Plus,
			TokenType::PlusEqual,
			TokenType::DoublePlus,
			TokenType::Minus,
			TokenType::MinusEqual,
			TokenType::DoubleMinus,
			TokenType::Star,
			TokenType::StarEqual,
			TokenType::Slash,
			TokenType::SlashEqual,
			TokenType::Ampersand,
			TokenType::DoubleAmpersand,
			TokenType::Pipe,
			TokenType::DoublePipe,
		};
		const auto result_tokens = Lexer::tokenize(string, &this->_diagnostics);

		ASSERT_EQ(expected_types.size(), result_tokens.size() - 1);
		for (size_t index = 0; index < expected_types.size(); ++index) {
			const auto& expected_token = Token(expected_types[index]);
			const auto& result_token   = result_tokens[index];
			ASSERT_EQ(expected_token, result_token);
			EXPECT_TRUE(result_tokens[index].no_value())
				<< "Expected no value, but got: " << std::string(result_tokens[index]);
		}

		ASSERT_TRUE(has_eof_token(result_tokens));
		ASSERT_TRUE(this->_diagnostics.empty());
	}

	TEST_F(LexerTest, Literals_Numbers_Integers)
	{
		const std::string_view string          = "0 54 1024 4098 -8192 1000000000000";
		const std::vector<i64> expected_values = {
			0, 54, 1024, 4098, -8192, 1000000000000,
		};
		const auto result_tokens = Lexer::tokenize(string, &this->_diagnostics);

		ASSERT_EQ(expected_values.size(), result_tokens.size() - 1);
		for (size_t index = 0; index < expected_values.size(); ++index) {
			const auto& expected_token = Token(TokenType::LiteralInteger, Value(expected_values[index]));
			const auto& result_token   = result_tokens[index];
			ASSERT_EQ(expected_token, result_token);
			ASSERT_TRUE(result_tokens[index].value.is<i64>())
				<< "Expected integer value, but got: " << std::string(result_tokens[index]);
			EXPECT_EQ(expected_token.value.get<i64>(), expected_values[index]);
		}

		ASSERT_TRUE(has_eof_token(result_tokens));
		ASSERT_TRUE(this->_diagnostics.empty());
	}

	TEST_F(LexerTest, Literals_Numbers_Integers_OutOfRange)
	{
		const std::string_view string        = "100000000000000000000";  // Number sufficient to be Out of Range.
		const auto             result_tokens = Lexer::tokenize(string, &this->_diagnostics);

		ASSERT_EQ(result_tokens.size(), 1);
		ASSERT_TRUE(has_eof_token(result_tokens));

		ASSERT_EQ(this->_diagnostics.size(), 1);
		ASSERT_EQ(this->_diagnostics[0].code(), DiagnosticCode::LexerValueOutOfRange);
	}

	TEST_F(LexerTest, Literals_Numbers_FloatingPoint)
	{
		const std::string_view string          = "0.0 -0.01 5.47 7.52 4098.0 -8192.32 1000000000000.0";
		const std::vector<f64> expected_values = { 0.0, -0.01, 5.47, 7.52, 4098.0, -8192.32, 1000000000000.0 };
		const auto             result_tokens   = Lexer::tokenize(string, &this->_diagnostics);

		ASSERT_EQ(expected_values.size(), result_tokens.size() - 1);
		for (size_t index = 0; index < expected_values.size(); ++index) {
			const auto& expected_token = Token(TokenType::LiteralFloat, Value(expected_values[index]));
			const auto& result_token   = result_tokens[index];
			ASSERT_EQ(expected_token, result_token);
			ASSERT_TRUE(result_tokens[index].value.is<f64>())
				<< "Expected floating point value, but got: " << std::string(result_tokens[index]);
			EXPECT_EQ(expected_token.value.get<f64>(), expected_values[index]);
		}

		ASSERT_TRUE(has_eof_token(result_tokens));
		ASSERT_TRUE(this->_diagnostics.empty());
	}

	TEST_F(LexerTest, Literals_Numbers_Mixed)
	{
		const std::string_view string          = "0 0.0 -4.14 5 8.72 20 -40 -1024.0 0.02";
		const std::vector      expected_tokens = {
            Token(TokenType::LiteralInteger, Value(static_cast<i64>(0))),
            Token(TokenType::LiteralFloat, Value(static_cast<f64>(0.0))),
            Token(TokenType::LiteralFloat, Value(static_cast<f64>(-4.14))),
            Token(TokenType::LiteralInteger, Value(static_cast<i64>(5))),
            Token(TokenType::LiteralFloat, Value(static_cast<f64>(8.72))),
            Token(TokenType::LiteralInteger, Value(static_cast<i64>(20))),
            Token(TokenType::LiteralInteger, Value(static_cast<i64>(-40))),
            Token(TokenType::LiteralFloat, Value(static_cast<f64>(-1024.0))),
            Token(TokenType::LiteralFloat, Value(static_cast<f64>(0.02))),
		};
		const auto result_tokens = Lexer::tokenize(string, &this->_diagnostics);

		ASSERT_EQ(expected_tokens.size(), result_tokens.size() - 1);
		for (size_t index = 0; index < expected_tokens.size(); ++index) {
			const auto& expected_token = expected_tokens[index];
			const auto& result_token   = result_tokens[index];
			ASSERT_EQ(expected_token, result_token);
			if (result_token.type() == TokenType::LiteralFloat) {
				ASSERT_TRUE(result_token.value.is<f64>() == expected_token.value.is<f64>());
				EXPECT_EQ(expected_token.value.get<f64>(), result_token.value.get<f64>());
			} else {
				ASSERT_TRUE(result_token.value.is<i64>() == expected_token.value.is<i64>());
				EXPECT_EQ(expected_token.value.get<i64>(), result_token.value.get<i64>());
			}
		}

		ASSERT_TRUE(has_eof_token(result_tokens));
		ASSERT_TRUE(this->_diagnostics.empty());
	}

	TEST_F(LexerTest, Literals_Strings)
	{
		const std::string_view         string          = R"("my_value""no space after previous" "520")";
		const std::vector<std::string> expected_values = { "my_value", "no space after previous", "520" };
		const auto                     result_tokens   = Lexer::tokenize(string, &this->_diagnostics);

		ASSERT_EQ(expected_values.size(), result_tokens.size() - 1);
		for (size_t index = 0; index < expected_values.size(); ++index) {
			const auto  expected_token = Token(TokenType::LiteralString, Value(expected_values[index]));
			const auto& result_token   = result_tokens[index];
			ASSERT_EQ(expected_token, result_token);
			ASSERT_TRUE(result_tokens[index].value.is<std::string>())
				<< "Expected string value, but got: " << std::string(result_tokens[index]);
			EXPECT_EQ(expected_token.value.get<std::string>(), expected_values[index]);
		}

		ASSERT_TRUE(has_eof_token(result_tokens));
		ASSERT_TRUE(this->_diagnostics.empty());
	}

	TEST_F(LexerTest, Literals_Strings_UnterminatedString)
	{
		const std::string_view string        = "\"this is an unterminated string, how sad :c";
		const auto             result_tokens = Lexer::tokenize(string, &this->_diagnostics);

		ASSERT_EQ(result_tokens.size(), 1);
		ASSERT_TRUE(has_eof_token(result_tokens));

		ASSERT_EQ(this->_diagnostics.size(), 1);
		ASSERT_EQ(this->_diagnostics[0].code(), DiagnosticCode::LexerUnterminatedString);
	}

	TEST_F(LexerTest, Compressed)
	{
		const std::string_view string          = "let variable:int=320;";
		const std::vector      expected_tokens = {
            Token(TokenType::KeywordLet), Token(TokenType::LiteralIdentifier, Value("variable")),
            Token(TokenType::Colon),      Token(TokenType::LiteralIdentifier, Value("int")),
            Token(TokenType::Equal),      Token(TokenType::LiteralInteger, Value(static_cast<i64>(320))),
            Token(TokenType::Semicolon),
		};
		const auto result_tokens = Lexer::tokenize(string, &this->_diagnostics);

		ASSERT_EQ(expected_tokens.size(), result_tokens.size() - 1);
		for (size_t index = 0; index < expected_tokens.size(); ++index) {
			const auto& expected_token = expected_tokens[index];
			const auto& result_token   = result_tokens[index];
			ASSERT_EQ(expected_token, result_token);
		}

		ASSERT_TRUE(has_eof_token(result_tokens));
		ASSERT_TRUE(this->_diagnostics.empty());
	}

	TEST_F(LexerTest, All)
	{
		const std::string_view string          = R"(
			fn main(some_var : int) :: void
			{
				let my_variable : str = "my_string";
				return 0;
			}
		)";
		const std::vector      expected_tokens = {
            Token(TokenType::KeywordFn),
            Token(TokenType::LiteralIdentifier, Value("main")),
            Token(TokenType::ParenLeft),
            Token(TokenType::LiteralIdentifier, Value("some_var")),
            Token(TokenType::Colon),
            Token(TokenType::LiteralIdentifier, Value("int")),
            Token(TokenType::ParenRight),
            Token(TokenType::DoubleColon),
            Token(TokenType::LiteralIdentifier, Value("void")),
            Token(TokenType::BraceLeft),
            Token(TokenType::KeywordLet),
            Token(TokenType::LiteralIdentifier, Value("my_variable")),
            Token(TokenType::Colon),
            Token(TokenType::LiteralIdentifier, Value("str")),
            Token(TokenType::Equal),
            Token(TokenType::LiteralString, Value("my_string")),
            Token(TokenType::Semicolon),
            Token(TokenType::KeywordReturn),
            Token(TokenType::LiteralInteger, Value(static_cast<i64>(0))),
            Token(TokenType::Semicolon),
            Token(TokenType::BraceRight),
		};
		const auto result_tokens = Lexer::tokenize(string, &this->_diagnostics);

		ASSERT_EQ(expected_tokens.size(), result_tokens.size() - 1);
		for (size_t index = 0; index < expected_tokens.size(); ++index) {
			const auto& expected_token = expected_tokens[index];
			const auto& result_token   = result_tokens[index];
			ASSERT_EQ(expected_token, result_token);
		}

		ASSERT_TRUE(has_eof_token(result_tokens));
		ASSERT_TRUE(this->_diagnostics.empty());
	}
}  // namespace soul::lexer::ut
