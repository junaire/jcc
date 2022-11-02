#include "jcc/lexer.h"

namespace jcc {

Keywords::Keywords() {
  // TODO(Jun): Use macros to reduce the work?
  keywords_.insert({"do", TokenKind::Do});
  keywords_.insert({"alignof", TokenKind::AlignOf});
  keywords_.insert({"auto", TokenKind::Auto});
  keywords_.insert({"break", TokenKind::Break});
  keywords_.insert({"case", TokenKind::Case});
  keywords_.insert({"char", TokenKind::Char});
  keywords_.insert({"const", TokenKind::Const});
  keywords_.insert({"continue", TokenKind::Continue});
  keywords_.insert({"default", TokenKind::Default});
  keywords_.insert({"do", TokenKind::Do});
  keywords_.insert({"double", TokenKind::Double});
  keywords_.insert({"else", TokenKind::Else});
  keywords_.insert({"enum", TokenKind::Enum});
  keywords_.insert({"extern", TokenKind::Extern});
  keywords_.insert({"float", TokenKind::Float});
  keywords_.insert({"for", TokenKind::For});
  keywords_.insert({"goto", TokenKind::Goto});
  keywords_.insert({"if", TokenKind::If});
  keywords_.insert({"inline", TokenKind::Inline});
  keywords_.insert({"int", TokenKind::Int});
  keywords_.insert({"long", TokenKind::Long});
  keywords_.insert({"register", TokenKind::Register});
  keywords_.insert({"restrict", TokenKind::Restrict});
  keywords_.insert({"return", TokenKind::Return});
  keywords_.insert({"short", TokenKind::Short});
  keywords_.insert({"signed", TokenKind::Signed});
  keywords_.insert({"sizeof", TokenKind::Sizeof});
  keywords_.insert({"static", TokenKind::Static});
  keywords_.insert({"struct", TokenKind::Struct});
  keywords_.insert({"switch", TokenKind::Switch});
  keywords_.insert({"typedef", TokenKind::Typedef});
  keywords_.insert({"union", TokenKind::Union});
  keywords_.insert({"unsigned", TokenKind::Unsigned});
  keywords_.insert({"void", TokenKind::Void});
  keywords_.insert({"while", TokenKind::While});
  keywords_.insert({"_Alignas", TokenKind::DashAlignas});
  keywords_.insert({"_Atomic", TokenKind::DashAtmoic});
  keywords_.insert({"_Bool", TokenKind::DashBool});
  keywords_.insert({"_Complex", TokenKind::DashComplex});
  keywords_.insert({"_Generic", TokenKind::DashGeneric});
  keywords_.insert({"_Imaginary", TokenKind::DashImaginary});
  keywords_.insert({"_Noreturn", TokenKind::DashNoReturn});
  keywords_.insert({"_Static_assert", TokenKind::DashStaticAssert});
  keywords_.insert({"_Thread_local", TokenKind::DashThreadLocal});
}

std::pair<bool, TokenKind> Keywords::matchKeyword(std::string_view identifier) {
  auto search = keywords_.find(identifier);
  if (search != keywords_.end()) {
    return {true, search->second};
  }
  return {false, TokenKind::Eof};
}

Token Lexer::Lex() {
  SkipWhitespace();
  switch (Peek()) {
    case '_':
    case 'a':
    case 'b':
    case 'c':
    case 'd':
    case 'e':
    case 'f':
    case 'g':
    case 'h':
    case 'i':
    case 'j':
    case 'k':
    case 'l':
    case 'm':
    case 'n':
    case 'o':
    case 'p':
    case 'q':
    case 'r':
    case 's':
    case 't':
    case 'u':
    case 'v':
    case 'w':
    case 'x':
    case 'y':
    case 'z':
    case 'A':
    case 'B':
    case 'C':
    case 'D':
    case 'E':
    case 'F':
    case 'G':
    case 'H':
    case 'I':
    case 'J':
    case 'K':
    case 'L':
    case 'M':
    case 'N':
    case 'O':
    case 'P':
    case 'Q':
    case 'R':
    case 'S':
    case 'T':
    case 'U':
    case 'V':
    case 'W':
    case 'X':
    case 'Y':
    case 'Z':
      return LexIdentifierOrKeyword();
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      return LexNumericConstant();
    case '[':
      return LexAtom(TokenKind::LeftSquare);
    case ']':
      return LexAtom(TokenKind::RightSquare);
    case '(':
      return LexAtom(TokenKind::LeftParen);
    case ')':
      return LexAtom(TokenKind::RightParen);
    case '{':
      return LexAtom(TokenKind::LeftBracket);
    case '}':
      return LexAtom(TokenKind::RightBracket);
    case '.':
      return LexAtom(TokenKind::Period);
    case '-': {
      SourceLocation loc{line_, column_, GetOffset()};
      if (TryConsume('>')) {
        return LexAtom(TokenKind::Arrow, loc);
      }
      if (TryConsume('-')) {
        return LexAtom(TokenKind::MinusMinus, loc);
      }
      if (TryConsume('=')) {
        return LexAtom(TokenKind::MinusEqual, loc);
      }
      return LexAtom(TokenKind::Minus, loc);
    }
    case '+': {
      SourceLocation loc{line_, column_, GetOffset()};
      if (TryConsume('+')) {
        return LexAtom(TokenKind::PlusPlus, loc);
      }
      if (TryConsume('=')) {
        return LexAtom(TokenKind::PlusEqual, loc);
      }
      return LexAtom(TokenKind::Plus, loc);
    }
    case '&': {
      SourceLocation loc{line_, column_, GetOffset()};

      if (TryConsume('&')) {
        return LexAtom(TokenKind::AmpersandAmpersand, loc);
      }
      return LexAtom(TokenKind::Ampersand, loc);
    }
    case '*': {
      SourceLocation loc{line_, column_, GetOffset()};
      if (TryConsume('=')) {
        return LexAtom(TokenKind::StarEqual, loc);
      }
      return LexAtom(TokenKind::Star, loc);
    }
    case '~':
      return LexAtom(TokenKind::Tilde);
    case '!': {
      SourceLocation loc{line_, column_, GetOffset()};
      if (TryConsume('=')) {
        return LexAtom(TokenKind::NotEqual, loc);
      }
      return LexAtom(TokenKind::Question, loc);
    }
    case '/': {
      SourceLocation loc{line_, column_, GetOffset()};
      if (TryConsume('*')) {
        // skip comments
        while (true) {
          SkipUntil('*', /*skip_match=*/true);
          TryConsume('/');
        }
      }
      if (TryConsume('=')) {
        return LexAtom(TokenKind::SlashEqual, loc);
      }
      return LexAtom(TokenKind::Slash, loc);
    }
    case '%': {
      SourceLocation loc{line_, column_, GetOffset()};
      if (TryConsume('=')) {
        return LexAtom(TokenKind::PercentEqual, loc);
      }
      return LexAtom(TokenKind::Percent, loc);
    }
    case '<': {
      SourceLocation loc{line_, column_, GetOffset()};
      if (TryConsume('<')) {
        if (TryConsume('=')) {
          return LexAtom(TokenKind::LeftShiftEqual, loc);
        }
        return LexAtom(TokenKind::LeftShift, loc);
      }
      if (TryConsume('=')) {
        return LexAtom(TokenKind::LessEqual);
      }
      return LexAtom(TokenKind::Less);
    }
    case '>': {
      SourceLocation loc{line_, column_, GetOffset()};
      if (TryConsume('>')) {
        if (TryConsume('=')) {
          return LexAtom(TokenKind::RightShiftEqual, loc);
        }
        return LexAtom(TokenKind::RightShift, loc);
      }
      if (TryConsume('=')) {
        return LexAtom(TokenKind::GreaterEqual, loc);
      }
      return LexAtom(TokenKind::Greater, loc);
    }
    case '=': {
      SourceLocation loc{line_, column_, GetOffset()};
      if (TryConsume('=')) {
        return LexAtom(TokenKind::EqualEqual);
      }
      return LexAtom(TokenKind::Equal);
    }
    case '^': {
      SourceLocation loc{line_, column_, GetOffset()};
      if (TryConsume('=')) {
        return LexAtom(TokenKind::CarretEqual, loc);
      }
      return LexAtom(TokenKind::Carret, loc);
    }
    case '|': {
      SourceLocation loc{line_, column_, GetOffset()};
      if (TryConsume('=')) {
        return LexAtom(TokenKind::PipeEqual, loc);
      }
      if (TryConsume('|')) {
        return LexAtom(TokenKind::PipePipe, loc);
      }
      return LexAtom(TokenKind::Pipe, loc);
    }
    case '?':
      return LexAtom(TokenKind::Question);
    case ':':
      return LexAtom(TokenKind::Colon);
    case ';':
      return LexAtom(TokenKind::Semi);
    case ',':
      return LexAtom(TokenKind::Comma);
    case '#': {
      if (TryConsume('#')) {
        return LexAtom(TokenKind::HashHash);
      }
      return LexAtom(TokenKind::Hash);
    }
    case '"':
      return LexStringLiteral();
    // TODO(Jun): How should we deal with single character
    case '\'':
      assert(0);
    case '\0':
      return LexAtom(TokenKind::Eof);
    default:
      assert(false && "Hit an unknown character!");
  }
}

void Lexer::SkipWhitespace() {
  while (std::isspace(Peek()) != 0) {
    Advance();
  }
}

bool Lexer::IsValidChar() const {
  bool flag = false;
  char cha = Peek();
  if (cha >= 'A' && cha <= 'Z') {
    flag = true;
  }

  if (cha >= 'a' && cha <= 'z') {
    flag = true;
  }

  if (cha == '_') {
    flag = true;
  }

  return flag;
}

bool Lexer::IsLineTerminator() const { return *buffer_ptr_ == '\n'; }

void Lexer::Advance() {
  assert(buffer_ptr_ != buffer_end_ + 1 && "Have already reached EOF!");
  buffer_ptr_++;
  // FIXME: This won't work with windows files.
  if (IsLineTerminator()) {
    line_ = 1;
    column_++;
    buffer_ptr_++;
  } else {
    line_++;
  }
}

char Lexer::Peek() const {
  if (buffer_ptr_ == buffer_end_) {
    return '\0';
  }
  return *buffer_ptr_;
}

char Lexer::PeekAhead(int offset) const {
  assert(buffer_ptr_ + offset != buffer_end_ && "Cannot peek over the buffer!");
  return *(buffer_ptr_ + offset);
}

std::size_t Lexer::GetOffset() const { return buffer_end_ - buffer_ptr_; }

bool Lexer::TryConsume(char cha) {
  if (PeekAhead() == cha) {
    Advance();
    return true;
  }
  return false;
}

void Lexer::SkipUntil(char cha, bool skip_match) {
  while (Peek() != cha) {
    Advance();
  }
  if (skip_match) {
    Advance();
  }
}

bool Lexer::HasDone() const { return buffer_ptr_ == buffer_end_ + 1; }

Token Lexer::LexAtom(TokenKind kind) {
  Token tok{kind, buffer_ptr_, 1, SourceLocation{line_, column_, GetOffset()}};
  Advance();
  return tok;
}

Token Lexer::LexAtom(TokenKind kind, SourceLocation loc) {
  Token tok{kind, buffer_ptr_, 1, loc};
  Advance();
  return tok;
}

// FIXME: can identifiers contain numbers?
Token Lexer::LexIdentifierOrKeyword() {
  SourceLocation loc{line_, column_, GetOffset()};
  const char* data = buffer_ptr_;
  Token::TokenSize len = 1;
  Advance();

  while (true) {
    Peek();
    if (!IsValidChar()) {
      break;
    }
    len++;
    Advance();
  }
  // the token may be a keyword.
  std::string_view tok{data, len};
  if (auto [isMatch, keyword] = keywords_.matchKeyword({data, len}); isMatch) {
    return {keyword, data, len, loc};
  }

  return {TokenKind::Identifier, data, len, loc};
}

Token Lexer::LexStringLiteral() {
  Advance();  // Eat the begin '"'
  SourceLocation loc{line_, column_, GetOffset()};
  const char* data = buffer_ptr_;
  Token::TokenSize len = 0;

  while (true) {
    if (Peek() == '"') {
      Advance();  // Eat the end '"'
      break;
    }
    len++;
    Advance();
  }

  return Token{TokenKind::StringLiteral, data, len, loc};
}

// TODO(Jun): extend this to support hex and exp
Token Lexer::LexNumericConstant() {
  const char* data = buffer_ptr_;
  Token::TokenSize len = 1;
  SourceLocation loc{line_, column_, GetOffset()};
  Advance();
  while (true) {
    char cha = Peek();
    if (std::isdigit(cha) != 0 || cha == '.') {
      len++;
      Advance();
    } else {
      break;
    }
  }
  return Token{TokenKind::NumericConstant, data, len, loc};
}
}  // namespace jcc
