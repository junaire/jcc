#include <iostream>
#include <string_view>

#include "gtest/gtest.h"
#include "jcc/lexer.h"
#include "jcc/token.h"

Lexer makeLexer(std::string_view code) {
  const char* start = code.data();
  const char* end = code.data() + code.size();
  return Lexer{start, end};
}

TEST(LexerTest, Basic) {
  {
    Lexer lexer = makeLexer("int main() {return 0;}");
    EXPECT_EQ(TokenKind::Int, lexer.lex().getKind());
    EXPECT_EQ(TokenKind::Identifier, lexer.lex().getKind());
    EXPECT_EQ(TokenKind::LeftParen, lexer.lex().getKind());
    EXPECT_EQ(TokenKind::RightParen, lexer.lex().getKind());
    EXPECT_EQ(TokenKind::LeftBracket, lexer.lex().getKind());
    EXPECT_EQ(TokenKind::Return, lexer.lex().getKind());
    EXPECT_EQ(TokenKind::NumericConstant, lexer.lex().getKind());
    EXPECT_EQ(TokenKind::Semi, lexer.lex().getKind());
    EXPECT_EQ(TokenKind::RightBracket, lexer.lex().getKind());
    EXPECT_EQ(TokenKind::Eof, lexer.lex().getKind());
  }

  {
    Lexer lexer = makeLexer("int foo() { int i = 1 + 2; i++;");

    EXPECT_EQ(TokenKind::Int, lexer.lex().getKind());
    EXPECT_EQ(TokenKind::Identifier, lexer.lex().getKind());
    EXPECT_EQ(TokenKind::LeftParen, lexer.lex().getKind());
    EXPECT_EQ(TokenKind::RightParen, lexer.lex().getKind());
    EXPECT_EQ(TokenKind::LeftBracket, lexer.lex().getKind());
    EXPECT_EQ(TokenKind::Int, lexer.lex().getKind());
    EXPECT_EQ(TokenKind::Identifier, lexer.lex().getKind());
    EXPECT_EQ(TokenKind::Equal, lexer.lex().getKind());
    EXPECT_EQ(TokenKind::NumericConstant, lexer.lex().getKind());
    EXPECT_EQ(TokenKind::Plus, lexer.lex().getKind());
    EXPECT_EQ(TokenKind::NumericConstant, lexer.lex().getKind());
    EXPECT_EQ(TokenKind::Semi, lexer.lex().getKind());
    EXPECT_EQ(TokenKind::Identifier, lexer.lex().getKind());
    EXPECT_EQ(TokenKind::PlusPlus, lexer.lex().getKind());
    EXPECT_EQ(TokenKind::Semi, lexer.lex().getKind());
    EXPECT_EQ(TokenKind::Eof, lexer.lex().getKind());
  }

  {
    Lexer lexer = makeLexer("i ");
    EXPECT_EQ(TokenKind::Identifier, lexer.lex().getKind());
    EXPECT_EQ(TokenKind::Eof, lexer.lex().getKind());
  }
}

TEST(LexerTest, WhileNotDone) {
  Lexer lexer = makeLexer("int    foo(){ return 0;  } ");
  while (!lexer.done()) {
    lexer.lex();
  }
}
