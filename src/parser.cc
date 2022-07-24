#include "jcc/parser.h"

#include "jcc/common.h"
#include "jcc/decl.h"
#include "jcc/expr.h"
#include "jcc/type.h"

Token Parser::currentToken() { return token_; }

void Parser::consumeToken() {
  if (cache_) {
    token_ = *cache_;
    cache_ = std::nullopt;
  } else {
    token_ = lexer_.lex();
  }
}

Token Parser::nextToken() {
  Token nextTok = lexer_.lex();
  cache_ = nextTok;
  return nextTok;
}

// Returns true if the next token is expected kind, doesn't really consume it.
bool Parser::tryConsumeToken(TokenKind expected) {
  return nextToken().getKind() == expected;
}

std::pair<Type, Attr> Parser::parseDeclSpec() {
  using enum TokenKind;
  Type type;
  Attr attr;
  while (currentToken().isTypename()) {
    if (currentToken()
            .isOneOf<Typedef, Static, Extern, Inline, DashThreadLocal>()) {
      // Check if storage class specifier is allowed in this context.
      if (currentToken().is<Typedef>()) {
        attr.isTypedef = true;
      } else if (currentToken().is<Static>()) {
        attr.isStatic = true;
      } else if (currentToken().is<Extern>()) {
        attr.isExtern = true;
      } else if (currentToken().is<Inline>()) {
        attr.isInline = true;
      } else {
        attr.isTls = true;
      }
      consumeToken();
    }
    // TODO(Jun):Throw an error if some keywords are combined incorrectly

    // Ignore some keywords with no effects like `auto`
    if (currentToken()
            .isOneOf<Const, Auto, Volatile, Register, Restrict,
                     DashNoReturn>()) {
      consumeToken();
    }

    // Deal with _Atomic and _Alignas
    if (currentToken().isOneOf<DashAtmoic, DashAlignas>()) {
      jcc_unreachable();
    }

    // Handle user defined types
    if (currentToken().isOneOf<Struct, Union, Typedef, Enum>()) {
      jcc_unreachable();
    }

    // Handle builtin types
    // FIXME: Some types can't be combined.
    if (currentToken().is<Void>()) {
      type.setKinds<Kind::Void>();
    } else if (currentToken().is<DashBool>()) {
      type.setKinds<Kind::Bool>();
    } else if (currentToken().is<Char>()) {
      type.setKinds<Kind::Char>();
    } else if (currentToken().is<Short>()) {
      type.setKinds<Kind::Short>();
    } else if (currentToken().is<Int>()) {
      type.setKinds<Kind::Int>();
    } else if (currentToken().is<Long>()) {
      type.setKinds<Kind::Long>();
    } else if (currentToken().is<Float>()) {
      type.setKinds<Kind::Float>();
    } else if (currentToken().is<Double>()) {
      type.setKinds<Kind::Double>();
    } else if (currentToken().is<Signed>()) {
      jcc_unreachable();
    } else if (currentToken().is<Unsigned>()) {
      jcc_unreachable();
    } else {
      jcc_unreachable();
    }
    consumeToken();
  }
  return std::make_pair(std::move(type), attr);
}

std::vector<VarDecl*> Parser::parseParams() {
  std::vector<VarDecl*> params;
  do {
    // To be implemented
  } while (currentToken().is<TokenKind::Comma>());
  return params;
}

// Create a new scope and parse
Stmt* Parser::parseFunctionBody() { return nullptr; }

Decl* Parser::parseFunction(Type type, const Attr& attr) {
  Token name(currentToken());
  // Parse function parameters
  consumeToken();  // Eat `(`
  std::vector<VarDecl*> parameters = parseParams();
  consumeToken();  // Eat `)`
  Stmt* body;
  if (currentToken().is<TokenKind::Semi>()) {
    consumeToken();  // Eat `;`
    body = nullptr;  // It's a declaration
  } else if (currentToken().is<TokenKind::LeftBracket>()) {
    body = parseFunctionBody();
    consumeToken();  // Eat `}`
  } else {
    jcc_unreachable();
  }

  // FIXME: Find a better approach to pass source loc
  // What should we do if there're redefinition?
  return FunctionDecl::create(ctx, {"Unknown", name.getLoc()}, name.getData(),
                              std::move(parameters), std::move(type), body);
}

// Function or a simple declaration
Decl* Parser::parseDeclaration(Type type, const Attr& attr) {
  if (!currentToken().is<TokenKind::Identifier>()) {
    jcc_unreachable();
  }
  if (tryConsumeToken(TokenKind::LeftParen)) {
    return parseFunction(std::move(type), attr);
  }

  // FIXME: Is it right?
  if (tryConsumeToken(TokenKind::Equal)) {
    // To be implemented
  }
  jcc_unreachable();
}

std::vector<Decl*> Parser::parseTranslateUnit() {
  std::vector<Decl*> decls;
  while (!currentToken().is<TokenKind::Eof>()) {
    auto [type, attr] = parseDeclSpec();
    // TODO(Jun): Handle typedefs
    if (attr.isTypedef) {
      jcc_unreachable();
    }
    decls.push_back(parseDeclaration(std::move(type), attr));
  }

  return decls;
}
