#pragma once

#include <memory>
#include <optional>
#include <unordered_map>

#include "jcc/ast_context.h"
#include "jcc/declarator.h"
#include "jcc/token.h"

class Decl;
class Expr;
class Parser;
class VarDecl;
class Stmt;
class Lexer;

struct Scope {
  explicit Scope(Parser& parser) : self(parser) {}
  Parser& self;
  std::unordered_map<std::string, Decl*> vars;
  std::unordered_map<std::string, Decl*> tags;
};

class Parser {
 public:
  explicit Parser(Lexer& lexer);

  std::vector<Decl*> ParseTranslateUnit();

  void SkipUntil(TokenKind kind);

  DeclSpec ParseDeclSpec();

  Declarator ParseDeclarator(DeclSpec& decl_spec);

  std::vector<Decl*> ParseFunctionOrVar(DeclSpec& decl_spec);

  std::vector<Decl*> ParseGlobalVariables(Declarator& declarator);

  Stmt* ParseStatement();

  Stmt* ParseCompoundStmt();

  Stmt* ParseReturnStmt();

  void addInitializer(VarDecl* var);

  Expr* ParseExpr();

  Expr* ParseAssignmentExpr();

  Expr* ParseCastExpr();

  Expr* ParseRhsOfBinaryExpr(Expr* lhs);

  Decl* ParseFunction(Declarator& declarator);

  std::unique_ptr<Type> ParseParams(std::unique_ptr<Type> type);

  std::unique_ptr<Type> ParseArrayDimensions(std::unique_ptr<Type> type);

  std::unique_ptr<Type> ParseTypeSuffix(std::unique_ptr<Type> type);

  std::unique_ptr<Type> ParsePointers(Declarator& declarator);

  std::unique_ptr<Type> ParseTypename();

  std::vector<VarDecl*> CreateParams(FunctionType* type);

  ASTContext& GetASTContext() { return ctx_; }

 private:
  Token CurrentToken();
  void ConsumeToken();
  bool tryConsumeToken(TokenKind expected);
  Token NextToken();
  Lexer& lexer_;
  Token token_;
  std::optional<Token> cache_;
  ASTContext ctx_;
  std::vector<Scope> scopes_;

  void EnterScope();
  void ExitScope();

  class ScopeRAII {
    Parser& self_;

   public:
    explicit ScopeRAII(Parser& self) : self_(self) { self_.EnterScope(); }
    ~ScopeRAII() { self_.ExitScope(); }
  };
};
