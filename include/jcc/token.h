#pragma once

#include <cassert>
#include <string>
#include <string_view>

#include "jcc/common.h"
#include "jcc/source_location.h"

namespace jcc {

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
  Volatile,
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
  Eof,
  Unspecified
};

// not all tokens need value, for some kinds like:
// operators, keywords, punctuation and etc, we just need store their kind,
// that's fairly enough. but for some tokens like identifiers, we also need
// store it's value.
//
// But for now, we can also hold the token's actual value, this will make
// debugging easier.
class Token {
  TokenKind kind_ = TokenKind::Unspecified;
  // I'm not sure if we need to do more for identifiers,
  // if so, we can use a variant.
  const char* data_;

  std::size_t length_;

  SourceLocation loc_;

 public:
  using TokenSize = std::size_t;

  Token() = default;
  Token(TokenKind kind, const char* data, std::size_t len,
        const SourceLocation& loc)
      : kind_(kind), data_(data), length_(len), loc_(loc) {}

  [[nodiscard]] TokenKind GetKind() const { return kind_; }

  template <TokenKind kind>
  [[nodiscard]] bool Is() const {
    return kind_ == kind;
  }

  template <TokenKind kind, TokenKind... kinds>
  [[nodiscard]] bool IsOneOf() {
    if (Is<kind>()) {
      return true;
    }
    if constexpr (sizeof...(kinds) > 0) {
      return IsOneOf<kinds...>();
    }
    return false;
  }

  [[nodiscard]] bool IsValid() const { return kind_ != TokenKind::Unspecified; }

  [[nodiscard]] std::string_view GetValue() const {
    assert(getLength() != 0 &&
           "Cannot get value from tokens that have no value!");
    return {GetData(), getLength()};
  }

  [[nodiscard]] std::string_view getKindName() const {
    switch (GetKind()) {
      case TokenKind::StringLiteral:
        return "StringLiteral";
      case TokenKind::NumericConstant:
        return "NumericConstant";
      case TokenKind::Identifier:
        return "Identifier";
      case TokenKind::Auto:
        return "Auto";
      case TokenKind::Break:
        return "Break";
      case TokenKind::Case:
        return "Case";
      case TokenKind::Char:
        return "char";
      case TokenKind::Const:
        return "const";
      case TokenKind::Continue:
        return "continue";
      case TokenKind::Default:
        return "default";
      case TokenKind::Do:
        return "Do";
      case TokenKind::Double:
        return "Double";
      case TokenKind::Else:
        return "Else";
      case TokenKind::Enum:
        return "Enum";
      case TokenKind::Extern:
        return "Extern";
      case TokenKind::Float:
        return "Float";
      case TokenKind::For:
        return "For";
      case TokenKind::Goto:
        return "Goto";
      case TokenKind::If:
        return "If";
      case TokenKind::Inline:
        return "Inline";
      case TokenKind::Int:
        return "Int";
      case TokenKind::Long:
        return "Long";
      case TokenKind::Register:
        return "Register";
      case TokenKind::Restrict:
        return "Restrict";
      case TokenKind::Return:
        return "Return";
      case TokenKind::Short:
        return "Short";
      case TokenKind::Signed:
        return "Signed";
      case TokenKind::Sizeof:
        return "Sizeof";
      case TokenKind::Static:
        return "Static";
      case TokenKind::Struct:
        return "Struct";
      case TokenKind::Switch:
        return "Switch";
      case TokenKind::Typedef:
        return "Typedef";
      case TokenKind::Union:
        return "Union";
      case TokenKind::Unsigned:
        return "Unsigned";
      case TokenKind::Void:
        return "Void";
      case TokenKind::While:
        return "While";
      case TokenKind::DashAlignas:
        return "_Alignas";
      case TokenKind::DashAtmoic:
        return "_Atomic";
      case TokenKind::DashBool:
        return "_Bool";
      case TokenKind::DashComplex:
        return "_Complex";
      case TokenKind::DashGeneric:
        return "_Generic";
      case TokenKind::DashImaginary:
        return "_Imaginary";
      case TokenKind::DashNoReturn:
        return "_Noreturn";
      case TokenKind::DashStaticAssert:
        return "_Static_assert";
      case TokenKind::DashThreadLocal:
        return "_Thread_local";

      case TokenKind::LeftSquare:
        return "[";
      case TokenKind::RightSquare:
        return "]";
      case TokenKind::LeftParen:
        return "(";
      case TokenKind::RightParen:
        return ")";
      case TokenKind::LeftBracket:
        return "{";
      case TokenKind::RightBracket:
        return "}";
      case TokenKind::Period:
        return ".";
      case TokenKind::Arrow:
        return "->";
      case TokenKind::PlusPlus:
        return "++";
      case TokenKind::MinusMinus:
        return "--";
      case TokenKind::Ampersand:
        return "&";
      case TokenKind::Star:
        return "*";
      case TokenKind::Plus:
        return "+";
      case TokenKind::Minus:
        return "-";
      case TokenKind::Tilde:
        return "~";
      case TokenKind::ExclamationMark:
        return "!";
      case TokenKind::Slash:
        return "/";
      case TokenKind::Percent:
        return "%";
      case TokenKind::LeftShift:
        return "<<";
      case TokenKind::RightShift:
        return ">>";
      case TokenKind::Less:
        return "<";
      case TokenKind::Greater:
        return ">";
      case TokenKind::LessEqual:
        return "<=";
      case TokenKind::GreaterEqual:
        return ">=";
      case TokenKind::EqualEqual:
        return "==";
      case TokenKind::NotEqual:
        return "!=";
      case TokenKind::Carret:
        return "^";
      case TokenKind::Pipe:
        return "|";
      case TokenKind::AmpersandAmpersand:
        return "&&";
      case TokenKind::PipePipe:
        return "||";
      case TokenKind::Question:
        return "?";
      case TokenKind::Colon:
        return ":";
      case TokenKind::Semi:
        return ";";
      case TokenKind::Ellipsis:
        return "...";
      case TokenKind::Equal:
        return "=";
      case TokenKind::StarEqual:
        return "*=";
      case TokenKind::SlashEqual:
        return "/=";
      case TokenKind::PercentEqual:
        return "%=";
      case TokenKind::PlusEqual:
        return "+=";
      case TokenKind::MinusEqual:
        return "-=";
      case TokenKind::LeftShiftEqual:
        return "<<=";
      case TokenKind::RightShiftEqual:
        return ">>=";
      case TokenKind::AmpersandEqual:
        return "&=";
      case TokenKind::CarretEqual:
        return "^=";
      case TokenKind::PipeEqual:
        return "|=";
      case TokenKind::Comma:
        return ",";
      case TokenKind::Hash:
        return "#";
      case TokenKind::HashHash:
        return "##";
      case TokenKind::Eof:
        return "Eof";
      default:
        jcc_unreachable("Unexpected charater when lexing!");
    }
  }

  [[nodiscard]] std::string GetAsString() const {
    return {GetData(), getLength()};
  }

  [[nodiscard]] std::size_t getLength() const { return length_; }

  [[nodiscard]] const char* GetData() const { return data_; }

  SourceLocation getLoc() { return loc_; }

  [[nodiscard]] bool IsTypename() const {
    // FIXME: the list seems to be not complete.
    switch (GetKind()) {
      case TokenKind::Auto:
      case TokenKind::Char:
      case TokenKind::Const:
      case TokenKind::Default:
      case TokenKind::Double:
      case TokenKind::Enum:
      case TokenKind::Extern:
      case TokenKind::Float:
      case TokenKind::Inline:
      case TokenKind::Int:
      case TokenKind::Long:
      case TokenKind::Register:
      case TokenKind::Restrict:
      case TokenKind::Short:
      case TokenKind::Signed:
      case TokenKind::Static:
      case TokenKind::Struct:
      case TokenKind::Typedef:
      case TokenKind::Union:
      case TokenKind::Unsigned:
      case TokenKind::Void:
      case TokenKind::Volatile:
      case TokenKind::DashAlignas:
      case TokenKind::DashAtmoic:
      case TokenKind::DashBool:
      case TokenKind::DashComplex:
      case TokenKind::DashNoReturn:
      case TokenKind::DashThreadLocal:
        return true;
      default:
        return false;
    }
  }
};
}  // namespace jcc
