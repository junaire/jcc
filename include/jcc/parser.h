#pragma once
#include <memory>
#include <optional>

#include "jcc/ast_context.h"
#include "jcc/decl.h"
#include "jcc/lexer.h"
#include "jcc/token.h"

class Parser {
 public:
  explicit Parser(Lexer& lexer) : lexer_(lexer) {}

  std::vector<Decl*> parseTranslateUnit();
  Decl* parseFunction(const DeclSpec& declSpec);
  Decl* parseDeclaration(const DeclSpec& declSpec);
  Stmt* parseFunctionBody();
  std::vector<VarDecl*> parseParams();
  Declarator parseDeclarator(const DeclSpec& declSpec);
  DeclSpec parseDeclSpec();
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
