#include "jcc/ast_context.h"
#include "jcc/decl.h"
#include "jcc/expr.h"
#include "jcc/stmt.h"

Decl* Decl::create(ASTContext& ctx, SourceRange loc) {
  void* mem = ctx.allocate<Decl>();
  return new (mem) Decl{std::move(loc)};
}

VarDecl* VarDecl::create(ASTContext& ctx, SourceRange loc, Stmt* init,
                         std::unique_ptr<Type> type, std::string name) {
  void* mem = ctx.allocate<VarDecl>();
  return new (mem)
      VarDecl{std::move(loc), init, std::move(type), std::move(name)};
}

FunctionDecl* FunctionDecl::create(ASTContext& ctx, SourceRange loc,
                                   std::string name, std::vector<VarDecl*> args,
                                   std::unique_ptr<Type> returnTy, Stmt* body) {
  void* mem = ctx.allocate<FunctionDecl>();
  return new (mem) FunctionDecl{std::move(loc), std::move(name),
                                std::move(args), std::move(returnTy), body};
}

RecordDecl* RecordDecl::create(ASTContext& ctx, SourceRange loc,
                               std::string name,
                               std::vector<VarDecl*> members) {
  void* mem = ctx.allocate<RecordDecl>();
  return new (mem)
      RecordDecl{std::move(loc), std::move(name), std::move(members)};
}

Expr* Expr::create(ASTContext& ctx, SourceRange loc) {
  void* mem = ctx.allocate<Expr>();
  return new (mem) Expr{std::move(loc)};
}

StringLiteral* StringLiteral::create(ASTContext& ctx, SourceRange loc,
                                     const char* literal) {
  void* mem = ctx.allocate<StringLiteral>();
  return new (mem) StringLiteral{std::move(loc), literal};
}

CharacterLiteral* CharacterLiteral::create(ASTContext& ctx, SourceRange loc,
                                           char value) {
  void* mem = ctx.allocate<CharacterLiteral>();
  return new (mem) CharacterLiteral{std::move(loc), value};
}

IntergerLiteral* IntergerLiteral::create(ASTContext& ctx, SourceRange loc,
                                         int value) {
  void* mem = ctx.allocate<IntergerLiteral>();
  return new (mem) IntergerLiteral{std::move(loc), value};
}

FloatingLiteral* FloatingLiteral::create(ASTContext& ctx, SourceRange loc,
                                         double value) {
  void* mem = ctx.allocate<FloatingLiteral>();
  return new (mem) FloatingLiteral{std::move(loc), value};
}

CallExpr* CallExpr::create(ASTContext& ctx, SourceRange loc, Expr* callee,
                           std::vector<Expr*> args) {
  void* mem = ctx.allocate<CallExpr>();
  return new (mem) CallExpr{std::move(loc), callee, std::move(args)};
}

UnaryExpr* UnaryExpr::create(ASTContext& ctx, SourceRange loc,
                             UnaryOperatorKind kind, Stmt* value) {
  void* mem = ctx.allocate<UnaryExpr>();
  return new (mem) UnaryExpr{std::move(loc), kind, value};
}

BinaryExpr* BinaryExpr::create(ASTContext& ctx, SourceRange loc,
                               BinaryOperatorKind kind, Expr* lhs, Expr* rhs) {
  void* mem = ctx.allocate<BinaryExpr>();
  return new (mem) BinaryExpr{std::move(loc), kind, lhs, rhs};
}

ArraySubscriptExpr* ArraySubscriptExpr::create(ASTContext& ctx, SourceRange loc,
                                               Expr* lhs, Expr* rhs) {
  void* mem = ctx.allocate<ArraySubscriptExpr>();
  return new (mem) ArraySubscriptExpr{std::move(loc), lhs, rhs};
}

MemberExpr* MemberExpr::create(ASTContext& ctx, SourceRange loc, Stmt* base,
                               Decl* member) {
  void* mem = ctx.allocate<MemberExpr>();
  return new (mem) MemberExpr{std::move(loc), base, member};
}

DeclRefExpr* DeclRefExpr::create(ASTContext& ctx, SourceRange loc, Decl* decl) {
  void* mem = ctx.allocate<DeclRefExpr>();
  return new (mem) DeclRefExpr{std::move(loc), decl};
}

ReturnStatement* ReturnStatement::create(ASTContext& ctx, SourceRange loc,
                                         Expr* returnExpr) {
  void* mem = ctx.allocate<ReturnStatement>();
  return new (mem) ReturnStatement(std::move(loc), returnExpr);
}

IfStatement* IfStatement::create(ASTContext& ctx, SourceRange loc,
                                 Expr* condition, Stmt* thenStmt,
                                 Stmt* elseStmt) {
  void* mem = ctx.allocate<IfStatement>();
  return new (mem) IfStatement(std::move(loc), condition, thenStmt, elseStmt);
}

DeclStatement* DeclStatement::create(ASTContext& ctx, SourceRange loc,
                                     std::vector<Decl*> decls) {
  void* mem = ctx.allocate<IfStatement>();
  return new (mem) DeclStatement(std::move(loc), std::move(decls));
}
DeclStatement* DeclStatement::create(ASTContext& ctx, SourceRange loc,
                                     Decl* decl) {
  void* mem = ctx.allocate<IfStatement>();
  return new (mem) DeclStatement(std::move(loc), decl);
}
