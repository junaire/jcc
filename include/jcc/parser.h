#pragma once

#include <optional>

#include "jcc/ast_context.h"
#include "jcc/declarator.h"
#include "jcc/expr.h"
#include "jcc/token.h"

class Decl;
class Expr;
class Parser;
class VarDecl;
class Stmt;
class Lexer;

enum class BinOpPreLevel {
  Unknown = 0,          // Not binary operator.
  Comma = 1,            // ,
  Assignment = 2,       // =, *=, /=, %=, +=, -=, <<=, >>=, &=, ^=, |=
  Conditional = 3,      // ?
  LogicalOr = 4,        // ||
  LogicalAnd = 5,       // &&
  InclusiveOr = 6,      // |
  ExclusiveOr = 7,      // ^
  And = 8,              // &
  Equality = 9,         // ==, !=
  Relational = 10,      //  >=, <=, >, <
  Spaceship = 11,       // <=>
  Shift = 12,           // <<, >>
  Additive = 13,        // -, +
  Multiplicative = 14,  // *, /, %
};

class Parser {
 public:
  explicit Parser(Lexer& lexer);

  std::vector<Decl*> ParseTranslateUnit();

  void SkipUntil(TokenKind kind, bool skip_match = false);

  DeclSpec ParseDeclSpec();

  Declarator ParseDeclarator(DeclSpec& decl_spec);

  std::vector<Decl*> ParseFunctionOrVar(DeclSpec& decl_spec);

  std::vector<Decl*> ParseDeclaration(Declarator& declarator);

  Stmt* ParseStatement();

  Stmt* ParseCompoundStmt();

  Stmt* ParseReturnStmt();

  Stmt* ParseIfStmt();

  Stmt* ParseWhileStmt();

  Expr* ParseExpr();

  Expr* ParseAssignmentExpr();

  Expr* ParseCastExpr();

  Expr* ParseRhsOfBinaryExpr(Expr* lhs, BinOpPreLevel min_prec);

  Decl* ParseFunction(Declarator& declarator);

  Type* ParseParams(Type* type);

  Type* ParseArrayDimensions(Type* type);

  Type* ParseTypeSuffix(Type* type);

  Type* ParsePointers(Declarator& declarator);

  Type* ParseTypename();

  std::vector<VarDecl*> CreateParams(FunctionType* type);

  ASTContext& GetASTContext() { return ctx_; }

  Decl* Lookup(const std::string& name) { return ctx_.Lookup(name); }

 private:
  Token CurrentToken();
  Token ConsumeToken();
  void MustConsumeToken(TokenKind expected);
  bool TryConsumeToken(TokenKind expected);
  Token NextToken();
  Lexer& lexer_;
  Token token_;
  std::optional<Token> cache_;
  ASTContext ctx_;

  class ScopeRAII {
    ASTContext& self_;

   public:
    explicit ScopeRAII(Parser& parser) : self_(parser.GetASTContext()) {
      self_.EnterScope();
    }
    ~ScopeRAII() { self_.ExitScope(); }
  };
};
