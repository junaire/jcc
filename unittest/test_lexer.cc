#include <string_view>

#include "gtest/gtest.h"
#include "jcc/lexer.h"
#include "jcc/token.h"

static bool IsTokenMatch(jcc::Lexer& lexer, jcc::TokenKind kind,
                         std::size_t line, std::size_t column) {
  auto tok = lexer.Lex();
  bool match = (kind == tok.GetKind() && line == tok.getLoc().GetLine() &&
                column == tok.getLoc().GetColumn());
  return match;
}

TEST(LexerTest, Empty) {
  jcc::Lexer lexer{""};
  EXPECT_EQ(jcc::TokenKind::Eof, lexer.Lex().GetKind());
}

TEST(LexerTest, OneLine) {
  {
    jcc::Lexer lexer{"int main() {return 0;}"};
    EXPECT_EQ(true, IsTokenMatch(lexer, jcc::TokenKind::Int, 1, 1));
    EXPECT_EQ(true, IsTokenMatch(lexer, jcc::TokenKind::Identifier, 5, 1));
    EXPECT_EQ(true, IsTokenMatch(lexer, jcc::TokenKind::LeftParen, 9, 1));
    EXPECT_EQ(true, IsTokenMatch(lexer, jcc::TokenKind::RightParen, 10, 1));
    EXPECT_EQ(true, IsTokenMatch(lexer, jcc::TokenKind::LeftBracket, 12, 1));
    EXPECT_EQ(true, IsTokenMatch(lexer, jcc::TokenKind::Return, 13, 1));
    EXPECT_EQ(true,
              IsTokenMatch(lexer, jcc::TokenKind::NumericConstant, 20, 1));
    EXPECT_EQ(true, IsTokenMatch(lexer, jcc::TokenKind::Semi, 21, 1));
    EXPECT_EQ(true, IsTokenMatch(lexer, jcc::TokenKind::RightBracket, 22, 1));
    EXPECT_EQ(true, IsTokenMatch(lexer, jcc::TokenKind::Eof, 23, 1));
  }

  {
    jcc::Lexer lexer{"int foo() { int i = 1 + 2; i++;"};
    EXPECT_EQ(true, IsTokenMatch(lexer, jcc::TokenKind::Int, 1, 1));
    EXPECT_EQ(true, IsTokenMatch(lexer, jcc::TokenKind::Identifier, 5, 1));
    EXPECT_EQ(true, IsTokenMatch(lexer, jcc::TokenKind::LeftParen, 8, 1));
    EXPECT_EQ(true, IsTokenMatch(lexer, jcc::TokenKind::RightParen, 9, 1));
    EXPECT_EQ(true, IsTokenMatch(lexer, jcc::TokenKind::LeftBracket, 11, 1));
    EXPECT_EQ(true, IsTokenMatch(lexer, jcc::TokenKind::Int, 13, 1));
    EXPECT_EQ(true, IsTokenMatch(lexer, jcc::TokenKind::Identifier, 17, 1));
    EXPECT_EQ(true, IsTokenMatch(lexer, jcc::TokenKind::Equal, 19, 1));
    EXPECT_EQ(true,
              IsTokenMatch(lexer, jcc::TokenKind::NumericConstant, 21, 1));
    EXPECT_EQ(true, IsTokenMatch(lexer, jcc::TokenKind::Plus, 23, 1));
    EXPECT_EQ(true,
              IsTokenMatch(lexer, jcc::TokenKind::NumericConstant, 25, 1));
    EXPECT_EQ(true, IsTokenMatch(lexer, jcc::TokenKind::Semi, 26, 1));
    EXPECT_EQ(true, IsTokenMatch(lexer, jcc::TokenKind::Identifier, 28, 1));
    EXPECT_EQ(true, IsTokenMatch(lexer, jcc::TokenKind::PlusPlus, 29, 1));
    EXPECT_EQ(true, IsTokenMatch(lexer, jcc::TokenKind::Semi, 31, 1));
    EXPECT_EQ(true, IsTokenMatch(lexer, jcc::TokenKind::Eof, 32, 1));
  }

  {
    jcc::Lexer lexer{"i "};

    EXPECT_EQ(true, IsTokenMatch(lexer, jcc::TokenKind::Identifier, 1, 1));
    EXPECT_EQ(true, IsTokenMatch(lexer, jcc::TokenKind::Eof, 3, 1));
  }
}

TEST(LexerTest, MultipleLine) {
  jcc::Lexer lexer{
      R"(int main()
{
int i = 0;
})"};

  EXPECT_EQ(true, IsTokenMatch(lexer, jcc::TokenKind::Int, 1, 1));
  EXPECT_EQ(true, IsTokenMatch(lexer, jcc::TokenKind::Identifier, 5, 1));
  EXPECT_EQ(true, IsTokenMatch(lexer, jcc::TokenKind::LeftParen, 9, 1));
  EXPECT_EQ(true, IsTokenMatch(lexer, jcc::TokenKind::RightParen, 10, 1));
  EXPECT_EQ(true, IsTokenMatch(lexer, jcc::TokenKind::LeftBracket, 1, 2));
  EXPECT_EQ(true, IsTokenMatch(lexer, jcc::TokenKind::Int, 1, 3));
  EXPECT_EQ(true, IsTokenMatch(lexer, jcc::TokenKind::Identifier, 5, 3));
  EXPECT_EQ(true, IsTokenMatch(lexer, jcc::TokenKind::Equal, 7, 3));
  EXPECT_EQ(true, IsTokenMatch(lexer, jcc::TokenKind::NumericConstant, 9, 3));
  EXPECT_EQ(true, IsTokenMatch(lexer, jcc::TokenKind::Semi, 10, 3));
  EXPECT_EQ(true, IsTokenMatch(lexer, jcc::TokenKind::RightBracket, 1, 4));
  EXPECT_EQ(true, IsTokenMatch(lexer, jcc::TokenKind::Eof, 2, 4));
}

TEST(LexerTest, WhileNotDone) {
  jcc::Lexer lexer{"int    foo(){ return 0;  } "};
  while (!lexer.HasDone()) {
    lexer.Lex();
  }
}
