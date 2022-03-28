#pragma once

#include <string_view>
#include <unordered_map>
#include <utility>

#include "jcc/token.h"

class Keywords {
  std::unordered_map<std::string_view, TokenKind> keywords_;

 public:
  Keywords();

  std::pair<bool, TokenKind> matchKeyword(std::string_view identifier);
};

// the lexer is not responsible for managing the buffer, instead it's an
// observer.
class Lexer {
 public:
  Lexer() = default;
  Lexer(const char* start, const char* end)
      : bufferStart(start), bufferEnd(end), bufferPtr(start){};

  Token lex();
  [[nodiscard]] bool done() const;

 private:
  Token lexAtom(TokenKind kind);
  Token lexStringLiteral();
  Token lexIdentifierOrKeyword();
  Token lexNumericConstant();

  void skipWhitespace();
  void skipUntil(char ch, bool skipItself = false);
  void advance();
  char peek() const;
  char peekAhead(int offset = 1) const;
  bool tryConsume(char ch);
  [[nodiscard]] bool isValidChar() const;

  const char* bufferStart;
  const char* bufferEnd;
  const char* bufferPtr;

  Keywords keywords_;
  // Do we need record the name of the current lexing file?
};
