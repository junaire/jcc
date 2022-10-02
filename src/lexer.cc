#include "jcc/lexer.h"

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

Token Lexer::lex() {
  skipWhitespace();
  switch (peek()) {
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
      return lexIdentifierOrKeyword();
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
      return lexNumericConstant();
    case '[':
      return lexAtom(TokenKind::LeftSquare);
    case ']':
      return lexAtom(TokenKind::RightSquare);
    case '(':
      return lexAtom(TokenKind::LeftParen);
    case ')':
      return lexAtom(TokenKind::RightParen);
    case '{':
      return lexAtom(TokenKind::LeftBracket);
    case '}':
      return lexAtom(TokenKind::RightBracket);
    case '.':
      return lexAtom(TokenKind::Period);
    case '-': {
      SourceLocation loc{line_, column_, getOffset()};
      if (tryConsume('>')) {
        return lexAtom(TokenKind::Arrow, loc);
      }
      if (tryConsume('-')) {
        return lexAtom(TokenKind::MinusMinus, loc);
      }
      if (tryConsume('=')) {
        return lexAtom(TokenKind::MinusEqual, loc);
      }
      return lexAtom(TokenKind::Minus, loc);
    }
    case '+': {
      SourceLocation loc{line_, column_, getOffset()};
      if (tryConsume('+')) {
        return lexAtom(TokenKind::PlusPlus, loc);
      }
      if (tryConsume('=')) {
        return lexAtom(TokenKind::PlusEqual, loc);
      }
      return lexAtom(TokenKind::Plus, loc);
    }
    case '&': {
      SourceLocation loc{line_, column_, getOffset()};

      if (tryConsume('&')) {
        return lexAtom(TokenKind::AmpersandAmpersand, loc);
      }
      return lexAtom(TokenKind::Ampersand, loc);
    }
    case '*': {
      SourceLocation loc{line_, column_, getOffset()};
      if (tryConsume('=')) {
        return lexAtom(TokenKind::StarEqual, loc);
      }
      return lexAtom(TokenKind::Star, loc);
    }
    case '~':
      return lexAtom(TokenKind::Tilde);
    case '!': {
      SourceLocation loc{line_, column_, getOffset()};
      if (tryConsume('=')) {
        return lexAtom(TokenKind::NotEqual, loc);
      }
      return lexAtom(TokenKind::Question, loc);
    }
    case '/': {
      SourceLocation loc{line_, column_, getOffset()};
      if (tryConsume('*')) {
        // skip comments
        while (true) {
          skipUntil('*', /*skipItself=*/true);
          tryConsume('/');
        }
      }
      if (tryConsume('=')) {
        return lexAtom(TokenKind::SlashEqual, loc);
      }
      return lexAtom(TokenKind::Slash, loc);
    }
    case '%': {
      SourceLocation loc{line_, column_, getOffset()};
      if (tryConsume('=')) {
        return lexAtom(TokenKind::PercentEqual, loc);
      }
      return lexAtom(TokenKind::Percent, loc);
    }
    case '<': {
      SourceLocation loc{line_, column_, getOffset()};
      if (tryConsume('<')) {
        if (tryConsume('=')) {
          return lexAtom(TokenKind::LeftShiftEqual, loc);
        }
        return lexAtom(TokenKind::LeftShift, loc);
      }
      if (tryConsume('=')) {
        return lexAtom(TokenKind::LessEqual);
      }
      return lexAtom(TokenKind::Less);
    }
    case '>': {
      SourceLocation loc{line_, column_, getOffset()};
      if (tryConsume('>')) {
        if (tryConsume('=')) {
          return lexAtom(TokenKind::RightShiftEqual, loc);
        }
        return lexAtom(TokenKind::RightShift, loc);
      }
      if (tryConsume('=')) {
        return lexAtom(TokenKind::GreaterEqual, loc);
      }
      return lexAtom(TokenKind::Greater, loc);
    }
    case '=': {
      SourceLocation loc{line_, column_, getOffset()};
      if (tryConsume('=')) {
        return lexAtom(TokenKind::EqualEqual);
      }
      return lexAtom(TokenKind::Equal);
    }
    case '^': {
      SourceLocation loc{line_, column_, getOffset()};
      if (tryConsume('=')) {
        return lexAtom(TokenKind::CarretEqual, loc);
      }
      return lexAtom(TokenKind::Carret, loc);
    }
    case '|': {
      SourceLocation loc{line_, column_, getOffset()};
      if (tryConsume('=')) {
        return lexAtom(TokenKind::PipeEqual, loc);
      }
      if (tryConsume('|')) {
        return lexAtom(TokenKind::PipePipe, loc);
      }
      return lexAtom(TokenKind::Pipe, loc);
    }
    case '?':
      return lexAtom(TokenKind::Question);
    case ':':
      return lexAtom(TokenKind::Colon);
    case ';':
      return lexAtom(TokenKind::Semi);
    case ',':
      return lexAtom(TokenKind::Comma);
    case '#': {
      if (tryConsume('#')) {
        return lexAtom(TokenKind::HashHash);
      }
      return lexAtom(TokenKind::Hash);
    }
    case '"':
      return lexStringLiteral();
    // TODO(Jun): How should we deal with single character
    case '\'':
      assert(0);
    case '\0':
      return lexAtom(TokenKind::Eof);
    default:
      assert(false && "Hit an unknown character!");
  }
}

void Lexer::skipWhitespace() {
  while (std::isspace(peek()) != 0) {
    advance();
  }
}

bool Lexer::isValidChar() const {
  bool flag = false;
  char c = peek();
  if (c >= 'A' && c <= 'Z') {
    flag = true;
  }

  if (c >= 'a' && c <= 'z') {
    flag = true;
  }

  if (c == '_') {
    flag = true;
  }

  return flag;
}

bool Lexer::isLineTerminator() const { return *bufferPtr == '\n'; }

void Lexer::advance() {
  assert(bufferPtr != bufferEnd + 1 && "Have already reached EOF!");
  bufferPtr++;
  // FIXME: This won't work with windows files.
  if (isLineTerminator()) {
    line_ = 1;
    column_++;
    bufferPtr++;
  } else {
    line_++;
  }
}

char Lexer::peek() const {
  if (bufferPtr == bufferEnd) {
    return '\0';
  }
  return *bufferPtr;
}

char Lexer::peekAhead(int offset) const {
  assert(bufferPtr + offset != bufferEnd && "Cannot peek over the buffer!");
  return *(bufferPtr + offset);
}

std::size_t Lexer::getOffset() const { return bufferEnd - bufferPtr; }

bool Lexer::tryConsume(char ch) {
  if (peekAhead() == ch) {
    advance();
    return true;
  }
  return false;
}

void Lexer::skipUntil(char ch, bool skipItself) {
  while (peek() != ch) {
    advance();
  }
  if (skipItself) {
    advance();
  }
}

bool Lexer::done() const { return bufferPtr == bufferEnd + 1; }

Token Lexer::lexAtom(TokenKind kind) {
  Token tok{kind, bufferPtr, 1, SourceLocation{line_, column_, getOffset()}};
  advance();
  return tok;
}

Token Lexer::lexAtom(TokenKind kind, SourceLocation loc) {
  Token tok{kind, bufferPtr, 1, loc};
  advance();
  return tok;
}

// FIXME: can identifiers contain numbers?
Token Lexer::lexIdentifierOrKeyword() {
  SourceLocation loc{line_, column_, getOffset()};
  const char* data = bufferPtr;
  Token::TokenSize len = 1;
  advance();

  while (true) {
    peek();
    if (!isValidChar()) {
      break;
    }
    len++;
    advance();
  }
  // the token may be a keyword.
  std::string_view tok{data, len};
  if (auto [isMatch, keyword] = keywords_.matchKeyword({data, len}); isMatch) {
    return {keyword, data, len, loc};
  }

  return {TokenKind::Identifier, data, len, loc};
}

Token Lexer::lexStringLiteral() {
  advance();
  SourceLocation loc{line_, column_, getOffset()};
  const char* data = bufferPtr;
  Token::TokenSize len = 1;

  while (!tryConsume('"')) {
    len++;
  }
  advance();

  return Token{TokenKind::StringLiteral, data, len, loc};
}

// TODO(Jun): extend this to support hex and exp
Token Lexer::lexNumericConstant() {
  const char* data = bufferPtr;
  Token::TokenSize len = 1;
  SourceLocation loc{line_, column_, getOffset()};
  advance();
  while (true) {
    char c = peek();
    if (std::isdigit(c) != 0 || c == '.') {
      len++;
      advance();
    } else {
      break;
    }
  }
  return Token{TokenKind::NumericConstant, data, len, loc};
}
