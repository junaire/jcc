#include <fmt/format.h>

#include "jcc/ast_context.h"
#include "jcc/decl.h"
#include "jcc/expr.h"
#include "jcc/stmt.h"

static void insertIndent(int n) {
  for (int i = 0; i < n; i++) {
    fmt::print(" ");
  }
}

VarDecl* VarDecl::create(ASTContext& ctx, SourceRange loc, Stmt* init,
                         std::unique_ptr<Type> type, std::string name) {
  void* mem = ctx.allocate<VarDecl>();
  return new (mem)
      VarDecl{std::move(loc), init, std::move(type), std::move(name)};
}

void VarDecl::dump(int indent) const {
  insertIndent(indent);
  fmt::print("VarDecl\n");
  fmt::print("  {}\n", getName());
  init_->dump(indent + 2);
}

FunctionDecl* FunctionDecl::create(ASTContext& ctx, SourceRange loc,
                                   std::string name, std::vector<VarDecl*> args,
                                   std::unique_ptr<Type> returnTy, Stmt* body) {
  void* mem = ctx.allocate<FunctionDecl>();
  return new (mem) FunctionDecl{std::move(loc), std::move(name),
                                std::move(args), std::move(returnTy), body};
}

void FunctionDecl::dump(int indent) const {
  insertIndent(indent);
  fmt::print("FunctionDecl\n");
  fmt::print("  {}\n", getName());
  fmt::print("  Args({})\n", getParamNum());
  for (const auto* arg : args_) {
    arg->dump(indent + 2);
  }
  body_->dump(indent + 2);
}

RecordDecl* RecordDecl::create(ASTContext& ctx, SourceRange loc,
                               std::string name,
                               std::vector<VarDecl*> members) {
  void* mem = ctx.allocate<RecordDecl>();
  return new (mem)
      RecordDecl{std::move(loc), std::move(name), std::move(members)};
}

void RecordDecl::dump(int indent) const {
  insertIndent(indent);
  fmt::print("RecordDecl\n");
  fmt::print("  {}\n", getName());
  fmt::print("  Args({})\n", getMemberNum());
  for (const auto* member : members_) {
    member->dump(indent + 2);
  }
}
StringLiteral* StringLiteral::create(ASTContext& ctx, SourceRange loc,
                                     const char* literal) {
  void* mem = ctx.allocate<StringLiteral>();
  return new (mem) StringLiteral{std::move(loc), literal};
}

void StringLiteral::dump(int indent) const {
  insertIndent(indent);
  fmt::print("StringLiteral: {}\n", getValue());
}

CharacterLiteral* CharacterLiteral::create(ASTContext& ctx, SourceRange loc,
                                           char value) {
  void* mem = ctx.allocate<CharacterLiteral>();
  return new (mem) CharacterLiteral{std::move(loc), value};
}

void CharacterLiteral::dump(int indent) const {
  insertIndent(indent);
  fmt::print("CharacterLiteral: {}\n", getValue());
}

IntergerLiteral* IntergerLiteral::create(ASTContext& ctx, SourceRange loc,
                                         int value) {
  void* mem = ctx.allocate<IntergerLiteral>();
  return new (mem) IntergerLiteral{std::move(loc), value};
}

void IntergerLiteral::dump(int indent) const {
  insertIndent(indent);
  fmt::print("IntergerLiteral: {}\n", getValue());
}

FloatingLiteral* FloatingLiteral::create(ASTContext& ctx, SourceRange loc,
                                         double value) {
  void* mem = ctx.allocate<FloatingLiteral>();
  return new (mem) FloatingLiteral{std::move(loc), value};
}

void FloatingLiteral::dump(int indent) const {
  insertIndent(indent);
  fmt::print("FloatingLiteral: {}\n", getValue());
}

CallExpr* CallExpr::create(ASTContext& ctx, SourceRange loc, Expr* callee,
                           std::vector<Expr*> args) {
  void* mem = ctx.allocate<CallExpr>();
  return new (mem) CallExpr{std::move(loc), callee, std::move(args)};
}

void CallExpr::dump(int indent) const { jcc_unreachable(); }

UnaryExpr* UnaryExpr::create(ASTContext& ctx, SourceRange loc,
                             UnaryOperatorKind kind, Stmt* value) {
  void* mem = ctx.allocate<UnaryExpr>();
  return new (mem) UnaryExpr{std::move(loc), kind, value};
}

void UnaryExpr::dump(int indent) const { jcc_unreachable(); }

BinaryExpr* BinaryExpr::create(ASTContext& ctx, SourceRange loc,
                               BinaryOperatorKind kind, Expr* lhs, Expr* rhs) {
  void* mem = ctx.allocate<BinaryExpr>();
  return new (mem) BinaryExpr{std::move(loc), kind, lhs, rhs};
}

void BinaryExpr::dump(int indent) const { jcc_unreachable(); }

ArraySubscriptExpr* ArraySubscriptExpr::create(ASTContext& ctx, SourceRange loc,
                                               Expr* lhs, Expr* rhs) {
  void* mem = ctx.allocate<ArraySubscriptExpr>();
  return new (mem) ArraySubscriptExpr{std::move(loc), lhs, rhs};
}

void ArraySubscriptExpr::dump(int indent) const { jcc_unreachable(); }

MemberExpr* MemberExpr::create(ASTContext& ctx, SourceRange loc, Stmt* base,
                               Decl* member) {
  void* mem = ctx.allocate<MemberExpr>();
  return new (mem) MemberExpr{std::move(loc), base, member};
}

void MemberExpr::dump(int indent) const { jcc_unreachable(); }

DeclRefExpr* DeclRefExpr::create(ASTContext& ctx, SourceRange loc, Decl* decl) {
  void* mem = ctx.allocate<DeclRefExpr>();
  return new (mem) DeclRefExpr{std::move(loc), decl};
}

void DeclRefExpr::dump(int indent) const { jcc_unreachable(); }

ReturnStatement* ReturnStatement::create(ASTContext& ctx, SourceRange loc,
                                         Expr* returnExpr) {
  void* mem = ctx.allocate<ReturnStatement>();
  return new (mem) ReturnStatement(std::move(loc), returnExpr);
}

void ReturnStatement::dump(int indent) const { jcc_unreachable(); }

IfStatement* IfStatement::create(ASTContext& ctx, SourceRange loc,
                                 Expr* condition, Stmt* thenStmt,
                                 Stmt* elseStmt) {
  void* mem = ctx.allocate<IfStatement>();
  return new (mem) IfStatement(std::move(loc), condition, thenStmt, elseStmt);
}

void IfStatement::dump(int indent) const { jcc_unreachable(); }

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

void DeclStatement::dump(int indent) const { jcc_unreachable(); }
