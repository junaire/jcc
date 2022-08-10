#pragma once

#include <memory>
#include <optional>

#include "jcc/ast_context.h"
#include "jcc/decl.h"
#include "jcc/lexer.h"
#include "jcc/token.h"
#include "jcc/declarator.h"


class Parser {
 public:
  explicit Parser(Lexer& lexer) : lexer_(lexer) {}

  std::vector<Decl*> parseTranslateUnit();

  DeclSpec parseDeclSpec();

  Declarator parseDeclarator(DeclSpec& declSpec);

  std::vector<Decl*> parseDeclaration(DeclSpec& declSpec);

  std::vector<Decl*> parseGlobalVariables(DeclSpec& declSpec);

  Decl* parseFunction(DeclSpec& declSpec);

  Stmt* parseFunctionBody();

  std::vector<VarDecl*> parseParams();

  std::unique_ptr<Type> parseTypeSuffix(std::unique_ptr<Type> type);

  std::unique_ptr<Type> parsePointers(Declarator& declrator);

  std::unique_ptr<Type> parseTypename();

  ASTContext& getASTContext() { return ctx_; }

 private:
  Token currentToken();
  void consumeToken();
  bool tryConsumeToken(TokenKind expected);
  Token nextToken();
  Lexer& lexer_;
  Token token_;
  std::optional<Token> cache_;
  ASTContext ctx_;
};
