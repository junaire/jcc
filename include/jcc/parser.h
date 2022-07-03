#pragma once

#include "jcc/lexer.h"
#include "jcc/token.h"

class Expr;
class Stmt;

class Parser {
 public:
  explicit Parser(Lexer& lexer) : lexer_(lexer) {}

  Expr* parseTranslateUnit();
  Expr* parseTopLevelDecl();

  Stmt* parseReturnStmt();

 private:
  Token currentToken();
  Token consumeToken();
  Token nextToken();
  Lexer& lexer_;
  Token token_;
};
