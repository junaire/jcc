#pragma once
#include <string>
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
  std::string fileName_;
  const char* bufferStart;
  const char* bufferEnd;
  const char* bufferPtr;

  std::size_t line_;
  std::size_t column_;

  Keywords keywords_;

 public:
  explicit Lexer(std::string_view source)
      : fileName_("<Buffer>"),
        bufferStart(source.begin()),
        bufferEnd(source.end()),
        bufferPtr(source.begin()),
        line_(1),
        column_(1){};

  Lexer(std::string name, std::string_view source)
      : fileName_(std::move(name)),
        bufferStart(source.begin()),
        bufferEnd(source.end()),
        bufferPtr(source.begin()),
        line_(1),
        column_(1){};

  Token lex();

  [[nodiscard]] bool done() const;

 private:
  Token lexAtom(TokenKind kind);
  Token lexAtom(TokenKind kind, SourceLocation loc);
  Token lexStringLiteral();
  Token lexIdentifierOrKeyword();
  Token lexNumericConstant();

  void skipWhitespace();
  void skipUntil(char ch, bool skipItself = false);
  void advance();
  char peek() const;
  char peekAhead(int offset = 1) const;
  bool tryConsume(char ch);
  std::size_t getOffset() const;
  [[nodiscard]] bool isValidChar() const;
  [[nodiscard]] bool isLineTerminator() const;
};
