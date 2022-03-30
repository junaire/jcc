#include <string_view>

#include "gtest/gtest.h"
#include "jcc/lexer.h"
#include "jcc/token.h"

TEST(LexerTest, Basic) {
  std::string_view code = R"(int main() {return 1;})";
  const char* start = code.data();
  const char* end = code.data() + code.size();
  Lexer lexer{start, end};
  EXPECT_EQ(TokenKind::Int, lexer.lex().getKind());
  EXPECT_EQ(TokenKind::Identifier, lexer.lex().getKind());
  EXPECT_EQ(TokenKind::LeftParen, lexer.lex().getKind());
  EXPECT_EQ(TokenKind::RightParen, lexer.lex().getKind());
  EXPECT_EQ(TokenKind::LeftBracket, lexer.lex().getKind());
  EXPECT_EQ(TokenKind::Return, lexer.lex().getKind());
  EXPECT_EQ(TokenKind::NumericConstant, lexer.lex().getKind());
  EXPECT_EQ(TokenKind::Semi, lexer.lex().getKind());
  EXPECT_EQ(TokenKind::RightBracket, lexer.lex().getKind());
}
