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

  // FIXME: odd api
  std::pair<bool, TokenKind> matchKeyword(std::string_view identifier);
};

// the lexer is not responsible for managing the buffer, instead it's an
// observer.
class Lexer {
  std::string fileName_;
  const char* bufferStart;
  const char* bufferEnd;
  const char* bufferPtr;

  std::size_t line_ = 1;
  std::size_t column_ = 1;

  Keywords keywords_;

 public:
  explicit Lexer(std::string_view source, std::string name = "<Buffer>")
      : fileName_(std::move(name)),
        // FIXME: we can't initialize const char* with string_view::begin() in
        // MSVC, not sure it's a bug :(
        bufferStart(source.data()),
        bufferEnd(source.data() + source.length()),
        bufferPtr(source.data()) {}

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
