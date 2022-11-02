#pragma once
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>

#include "jcc/token.h"

namespace jcc {
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
  std::string file_name_;
  const char* buffer_start_;
  const char* buffer_end_;
  const char* buffer_ptr_;

  std::size_t line_ = 1;
  std::size_t column_ = 1;

  Keywords keywords_;

 public:
  explicit Lexer(std::string_view source, std::string name = "<Buffer>")
      : file_name_(std::move(name)),
        // FIXME: we can't initialize const char* with string_view::begin(),
        // it's implementation defined.
        buffer_start_(source.data()),
        buffer_end_(source.data() + source.length()),
        buffer_ptr_(source.data()) {}

  Token Lex();

  [[nodiscard]] bool HasDone() const;

 private:
  Token LexAtom(TokenKind kind);
  Token LexAtom(TokenKind kind, SourceLocation loc);
  Token LexStringLiteral();
  Token LexIdentifierOrKeyword();
  Token LexNumericConstant();

  void SkipWhitespace();
  void SkipUntil(char cha, bool skip_match = false);
  void Advance();
  char Peek() const;
  char PeekAhead(int offset = 1) const;
  bool TryConsume(char cha);
  std::size_t GetOffset() const;
  [[nodiscard]] bool IsValidChar() const;
  [[nodiscard]] bool IsLineTerminator() const;
};
}  // namespace jcc
