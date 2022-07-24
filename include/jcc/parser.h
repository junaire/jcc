#pragma once
#include <optional>

#include "jcc/ast_context.h"
#include "jcc/lexer.h"
#include "jcc/token.h"

class Type;
class Expr;
class Stmt;
class Decl;
class VarDecl;

struct Attr {
  bool isTypedef = false;
  bool isStatic = false;
  bool isExtern = false;
  bool isInline = false;
  bool isTls = false;
  int align = 0;
};

class Parser {
 public:
  explicit Parser(Lexer& lexer) : lexer_(lexer) {}

  std::vector<Decl*> parseTranslateUnit();
  Decl* parseFunction(Type type, const Attr& attr);
  Decl* parseDeclaration(Type type, const Attr& attr);
  Stmt* parseFunctionBody();
  std::vector<VarDecl*> parseParams();
  void parseDeclarator();
  std::pair<Type, Attr> parseDeclSpec();

 private:
  Token currentToken();
  void consumeToken();
  bool tryConsumeToken(TokenKind expected);
  Token nextToken();
  Lexer& lexer_;
  Token token_;
  std::optional<Token> cache_;
  ASTContext ctx;
};
