#pragma once

#include <cassert>
#include <string>
#include <string_view>

// not all tokens need value, for some kinds like:
// operators, keywords, punctuation and etc, we just need store their kind,
// that's fairly enough. but for some tokens like identifiers, we also need
// store it's value.
//
// But for now, we can also hold the token's actual value, this will make
// debugging easier.
class Token {
 public:
  enum class TokenKind {
    StringLiteral,
    NumericConstant,
    Identifier,

    // Keywords
    AlignOf,
    Auto,
    Break,
    Case,
    Char,
    Const,
    Continue,
    Default,
    Do,
    Double,
    Else,
    Enum,
    Extern,
    Float,
    For,
    Goto,
    If,
    Inline,
    Int,
    Long,
    Register,
    Restrict,
    Return,
    Short,
    Signed,
    Sizeof,
    Static,
    Struct,
    Switch,
    Typedef,
    Union,
    Unsigned,
    Void,
    While,
    DashAlignas,
    DashAtmoic,
    DashBool,
    DashComplex,
    DashGeneric,
    DashImaginary,
    DashNoReturn,
    DashStaticAssert,
    DashThreadLocal,

    // Punctuators
    LeftSquare,          // [
    RightSquare,         // ]
    LeftParen,           // (
    RightParen,          // )
    LeftBracket,         // {
    RightBracket,        // }
    Period,              // .
    Arrow,               // ->
    PlusPlus,            // ++
    MinusMinus,          // --
    Ampersand,           // &
    Star,                // *
    Plus,                // +
    Minus,               // -
    Tilde,               // ~
    ExclamationMark,     // !
    Slash,               // /
    Percent,             // %
    LeftShift,           // <<
    RightShift,          // >>
    Less,                // <
    Greater,             // >
    LessEqual,           // <=
    GreaterEqual,        // >=
    EqualEqual,          // ==
    NotEqual,            // !=
    Carret,              // ^
    Pipe,                // |
    AmpersandAmpersand,  // &&
    PipePipe,            // ||
    Question,            // ?
    Colon,               // :
    Semi,                // ;
    Ellipsis,            // ...
    Equal,               // =
    StarEqual,           // *=
    SlashEqual,          // /=
    PercentEqual,        // %=
    PlusEqual,           // +=
    MinusEqual,          // -=
    LeftShiftEqual,      // <<=
    RightShiftEqual,     // >>=
    AmpersandEqual,      // &=
    CarretEqual,         // ^=
    PipeEqual,           // |=
    Comma,               // ,
    Hash,                // #
    HashHash,            // ##
    // Note that we're still missing some punctuators like:
    // <: :> <% %> %: %:%:
    // Ref: C11 6.4.6
    Eof
  };

  Token() = default;
  Token(TokenKind kind, std::string value)
      : kind_(kind), value_(std::move(value)) {}

  TokenKind getKind() { return kind_; }
  void setKind(TokenKind kind) { kind_ = kind; }

  [[nodiscard]] bool is(TokenKind kind) const { return kind_ == kind; }
  template <typename... Ts>
  [[nodiscard]] bool isOneOf(TokenKind kind, Ts... kinds) {
    // TODO(Jun): Unimplemented
    return false;
  }

  [[nodiscard]] std::string_view getValue() const {
    // TODO(Jun): Add assertions once we figure out how many token kinds we have
    return value_;
  }

 private:
  TokenKind kind_;
  std::string value_;
  // TODO(Jun): Add source location
};
