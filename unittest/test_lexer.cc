#include <string_view>

#include "gtest/gtest.h"
#include "jcc/lexer.h"
#include "jcc/token.h"

static bool IsTokenMatch(Lexer& lexer, TokenKind kind, std::size_t line,
                         std::size_t column) {
  auto tok = lexer.Lex();
  bool match = (kind == tok.GetKind() && line == tok.getLoc().GetLine() &&
                column == tok.getLoc().GetColumn());
  return match;
}

TEST(LexerTest, Empty) {
  Lexer lexer{""};
  EXPECT_EQ(TokenKind::Eof, lexer.Lex().GetKind());
}

TEST(LexerTest, OneLine) {
  {
    Lexer lexer{"int main() {return 0;}"};
    EXPECT_EQ(true, IsTokenMatch(lexer, TokenKind::Int, 1, 1));
    EXPECT_EQ(true, IsTokenMatch(lexer, TokenKind::Identifier, 5, 1));
    EXPECT_EQ(true, IsTokenMatch(lexer, TokenKind::LeftParen, 9, 1));
    EXPECT_EQ(true, IsTokenMatch(lexer, TokenKind::RightParen, 10, 1));
    EXPECT_EQ(true, IsTokenMatch(lexer, TokenKind::LeftBracket, 12, 1));
    EXPECT_EQ(true, IsTokenMatch(lexer, TokenKind::Return, 13, 1));
    EXPECT_EQ(true, IsTokenMatch(lexer, TokenKind::NumericConstant, 20, 1));
    EXPECT_EQ(true, IsTokenMatch(lexer, TokenKind::Semi, 21, 1));
    EXPECT_EQ(true, IsTokenMatch(lexer, TokenKind::RightBracket, 22, 1));
    EXPECT_EQ(true, IsTokenMatch(lexer, TokenKind::Eof, 23, 1));
  }

  {
    Lexer lexer{"int foo() { int i = 1 + 2; i++;"};
    EXPECT_EQ(true, IsTokenMatch(lexer, TokenKind::Int, 1, 1));
    EXPECT_EQ(true, IsTokenMatch(lexer, TokenKind::Identifier, 5, 1));
    EXPECT_EQ(true, IsTokenMatch(lexer, TokenKind::LeftParen, 8, 1));
    EXPECT_EQ(true, IsTokenMatch(lexer, TokenKind::RightParen, 9, 1));
    EXPECT_EQ(true, IsTokenMatch(lexer, TokenKind::LeftBracket, 11, 1));
    EXPECT_EQ(true, IsTokenMatch(lexer, TokenKind::Int, 13, 1));
    EXPECT_EQ(true, IsTokenMatch(lexer, TokenKind::Identifier, 17, 1));
    EXPECT_EQ(true, IsTokenMatch(lexer, TokenKind::Equal, 19, 1));
    EXPECT_EQ(true, IsTokenMatch(lexer, TokenKind::NumericConstant, 21, 1));
    EXPECT_EQ(true, IsTokenMatch(lexer, TokenKind::Plus, 23, 1));
    EXPECT_EQ(true, IsTokenMatch(lexer, TokenKind::NumericConstant, 25, 1));
    EXPECT_EQ(true, IsTokenMatch(lexer, TokenKind::Semi, 26, 1));
    EXPECT_EQ(true, IsTokenMatch(lexer, TokenKind::Identifier, 28, 1));
    EXPECT_EQ(true, IsTokenMatch(lexer, TokenKind::PlusPlus, 29, 1));
    EXPECT_EQ(true, IsTokenMatch(lexer, TokenKind::Semi, 31, 1));
    EXPECT_EQ(true, IsTokenMatch(lexer, TokenKind::Eof, 32, 1));
  }

  {
    Lexer lexer{"i "};

    EXPECT_EQ(true, IsTokenMatch(lexer, TokenKind::Identifier, 1, 1));
    EXPECT_EQ(true, IsTokenMatch(lexer, TokenKind::Eof, 3, 1));
  }
}

TEST(LexerTest, MultipleLine) {
  Lexer lexer{
      R"(int main()
{
int i = 0;
})"};

  EXPECT_EQ(true, IsTokenMatch(lexer, TokenKind::Int, 1, 1));
  EXPECT_EQ(true, IsTokenMatch(lexer, TokenKind::Identifier, 5, 1));
  EXPECT_EQ(true, IsTokenMatch(lexer, TokenKind::LeftParen, 9, 1));
  EXPECT_EQ(true, IsTokenMatch(lexer, TokenKind::RightParen, 10, 1));
  EXPECT_EQ(true, IsTokenMatch(lexer, TokenKind::LeftBracket, 1, 2));
  EXPECT_EQ(true, IsTokenMatch(lexer, TokenKind::Int, 1, 3));
  EXPECT_EQ(true, IsTokenMatch(lexer, TokenKind::Identifier, 5, 3));
  EXPECT_EQ(true, IsTokenMatch(lexer, TokenKind::Equal, 7, 3));
  EXPECT_EQ(true, IsTokenMatch(lexer, TokenKind::NumericConstant, 9, 3));
  EXPECT_EQ(true, IsTokenMatch(lexer, TokenKind::Semi, 10, 3));
  EXPECT_EQ(true, IsTokenMatch(lexer, TokenKind::RightBracket, 1, 4));
  EXPECT_EQ(true, IsTokenMatch(lexer, TokenKind::Eof, 2, 4));
}

TEST(LexerTest, WhileNotDone) {
  Lexer lexer{"int    foo(){ return 0;  } "};
  while (!lexer.HasDone()) {
    lexer.Lex();
  }
}
