#pragma once

#include <cassert>
#include <string>
#include <string_view>

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

// not all tokens need value, for some kinds like:
// operators, keywords, punctuation and etc, we just need store their kind,
// that's fairly enough. but for some tokens like identifiers, we also need
// store it's value.
//
// But for now, we can also hold the token's actual value, this will make
// debugging easier.
class Token {
 public:
  using TokenSize = std::size_t;
  Token() = default;
  explicit Token(TokenKind kind, const char* data, TokenSize len)
      : kind_(kind), data_(data), length_(len) {}
  explicit Token(TokenKind kind) : kind_(kind), data_(nullptr), length_(0) {}

  [[nodiscard]] TokenKind getKind() const { return kind_; }

  [[nodiscard]] bool is(TokenKind kind) const { return kind_ == kind; }
  template <typename... Ts>
  [[nodiscard]] bool isOneOf(TokenKind kind, Ts... kinds) {
    assert(0 && "Unimplemented!");
    if constexpr (sizeof...(kinds) > 0) {
    }
    return false;
  }

  [[nodiscard]] std::string_view getValue() const {
    assert(getLength() != 0 &&
           "Cannot get value from tokens that have no value!");
    return {getData(), getLength()};
  }

  [[nodiscard]] std::string_view getName() const {
    switch (getKind()) {
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
        assert(0);
    }
  }

  [[nodiscard]] TokenSize getLength() const { return length_; }

  [[nodiscard]] const char* getData() const { return data_; }

 private:
  TokenKind kind_;
  // I'm not sure if we need to do more for identifiers,
  // if so, we can use a variant.
  const char* data_;

  TokenSize length_;
  // TODO(Jun): Add source location
};
