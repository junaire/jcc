#include <fmt/format.h>

#include <string_view>

#include "fmt/core.h"
#include "jcc/ast_context.h"
#include "jcc/codegen.h"
#include "jcc/common.h"
#include "jcc/decl.h"
#include "jcc/expr.h"
#include "jcc/source_location.h"
#include "jcc/stmt.h"

namespace jcc {

#define GEN(Node) \
  void Node::GenCode(CodeGen& gen) { gen.Emit##Node(*this); }

GEN(VarDecl)
GEN(FunctionDecl)

GEN(CompoundStatement)
GEN(DeclStatement)
GEN(WhileStatement)
GEN(ForStatement)
GEN(IfStatement)
GEN(DoStatement)
GEN(SwitchStatement)
GEN(CaseStatement)
GEN(DefaultStatement)
GEN(ReturnStatement)
GEN(BreakStatement)
GEN(ContinueStatement)

GEN(DeclRefExpr)
GEN(ArraySubscriptExpr)
GEN(BinaryExpr)
GEN(UnaryExpr)
GEN(RecordDecl)
GEN(MemberExpr)
GEN(ExprStatement)
GEN(StringLiteral)
GEN(CharacterLiteral)
GEN(IntergerLiteral)
GEN(CallExpr)
GEN(FloatingLiteral)

static void InsertIndent(int n) {
  for (int i = 0; i < n; i++) {
    fmt::print(" ");
  }
}

VarDecl* VarDecl::Create(ASTContext& ctx, SourceRange loc, Stmt* init,
                         Type* type, std::string name) {
  void* mem = ctx.Allocate<VarDecl>();
  auto* var = new (mem) VarDecl(std::move(loc), init, type, std::move(name));
  ctx.GetCurScope().PushVar(var->GetName(), var);
  return var;
}

void VarDecl::dump(int indent) const {
  InsertIndent(indent);
  fmt::print("VarDecl: {}\n", GetName());

  // If we have init.
  if (init_ != nullptr) {
    init_->dump(indent + 2);
  }
}

FunctionDecl* FunctionDecl::Create(ASTContext& ctx, SourceRange loc,
                                   std::string name, std::vector<VarDecl*> args,
                                   Type* return_type, Stmt* body) {
  void* mem = ctx.Allocate<FunctionDecl>();
  auto* function = new (mem) FunctionDecl(std::move(loc), std::move(name),
                                          std::move(args), return_type, body);
  ctx.GetCurScope().PushVar(name, function);
  return function;
}

FunctionDecl* FunctionDecl::Create(ASTContext& ctx, SourceRange loc,
                                   const std::string& name, Type* return_type) {
  void* mem = ctx.Allocate<FunctionDecl>();
  auto* function = new (mem) FunctionDecl(std::move(loc), name, return_type);
  ctx.GetCurScope().PushVar(name, function);
  return function;
}

void FunctionDecl::dump(int indent) const {
  InsertIndent(indent);
  fmt::print("FunctionDecl: {}\n", GetName());
  InsertIndent(indent + 2);
  fmt::print("Args[{}]\n", GetParamNum());
  for (const auto* arg : args_) {
    arg->dump(indent + 2);
  }
  if (body_ != nullptr) {
    body_->dump(indent + 2);
  } else {
    InsertIndent(indent + 2);
    fmt::print("Body(empty)\n");
  }
}

RecordDecl* RecordDecl::Create(ASTContext& ctx, SourceRange loc,
                               std::string name,
                               std::vector<VarDecl*> members) {
  void* mem = ctx.Allocate<RecordDecl>();
  return new (mem)
      RecordDecl{std::move(loc), std::move(name), std::move(members)};
}

void RecordDecl::dump(int indent) const {
  InsertIndent(indent);
  fmt::print("RecordDecl: {}\n", GetName());
  for (const auto* member : members_) {
    member->dump(indent + 2);
  }
}
StringLiteral* StringLiteral::Create(ASTContext& ctx, SourceRange loc,
                                     std::string literal) {
  void* mem = ctx.Allocate<StringLiteral>();
  return new (mem) StringLiteral(std::move(loc), std::move(literal));
}

void StringLiteral::dump(int indent) const {
  InsertIndent(indent);
  fmt::print("StringLiteral: {}\n", GetValue());
}

CharacterLiteral* CharacterLiteral::Create(ASTContext& ctx, SourceRange loc,
                                           char value) {
  void* mem = ctx.Allocate<CharacterLiteral>();
  return new (mem) CharacterLiteral{std::move(loc), value};
}

void CharacterLiteral::dump(int indent) const {
  InsertIndent(indent);
  fmt::print("CharacterLiteral: {}\n", GetValue());
}

IntergerLiteral* IntergerLiteral::Create(ASTContext& ctx, SourceRange loc,
                                         int value) {
  void* mem = ctx.Allocate<IntergerLiteral>();
  return new (mem) IntergerLiteral{std::move(loc), value};
}

void IntergerLiteral::dump(int indent) const {
  InsertIndent(indent);
  fmt::print("IntergerLiteral: {}\n", GetValue());
}

FloatingLiteral* FloatingLiteral::Create(ASTContext& ctx, SourceRange loc,
                                         double value) {
  void* mem = ctx.Allocate<FloatingLiteral>();
  return new (mem) FloatingLiteral{std::move(loc), value};
}

void FloatingLiteral::dump(int indent) const {
  InsertIndent(indent);
  fmt::print("FloatingLiteral: {}\n", GetValue());
}

CallExpr* CallExpr::Create(ASTContext& ctx, SourceRange loc, Expr* callee,
                           std::vector<Expr*> args) {
  void* mem = ctx.Allocate<CallExpr>();
  return new (mem) CallExpr{std::move(loc), callee, std::move(args)};
}

void CallExpr::dump(int indent) const {
  InsertIndent(indent);
  fmt::print("CallExpr: {}\n", GetCallee()
                                   ->AsExpr<DeclRefExpr>()
                                   ->GetRefDecl()
                                   ->AsDecl<FunctionDecl>()
                                   ->GetName());
}

UnaryExpr* UnaryExpr::Create(ASTContext& ctx, SourceRange loc,
                             UnaryOperatorKind kind, Stmt* value) {
  void* mem = ctx.Allocate<UnaryExpr>();
  return new (mem) UnaryExpr{std::move(loc), kind, value};
}

static std::string_view PrintUnaryOpKind(UnaryOperatorKind kind) {
  switch (kind) {
    case UnaryOperatorKind::AddressOf:
      return "&";
    case UnaryOperatorKind::PostIncrement:
      return "++";
    case UnaryOperatorKind::PostDecrement:
      return "--";
    default:
      jcc_unimplemented();
  }
}
void UnaryExpr::dump(int indent) const {
  InsertIndent(indent);
  fmt::print("UnaryExpr({}):\n", PrintUnaryOpKind(kind_));
  value_->dump(indent + 2);
}

BinaryExpr* BinaryExpr::Create(ASTContext& ctx, SourceRange loc,
                               BinaryOperatorKind kind, Expr* lhs, Expr* rhs) {
  void* mem = ctx.Allocate<BinaryExpr>();
  return new (mem) BinaryExpr{std::move(loc), kind, lhs, rhs};
}

static std::string_view PrintBinaryOpKind(BinaryOperatorKind kind) {
  switch (kind) {
    case BinaryOperatorKind::Plus:
      return "+";
    case BinaryOperatorKind::Minus:
      return "-";
    case BinaryOperatorKind::Multiply:
      return "*";
    case BinaryOperatorKind::Divide:
      return "/";
    case BinaryOperatorKind::Greater:
      return ">";
    case BinaryOperatorKind::Less:
      return "<";
    case BinaryOperatorKind::Equal:
      return "=";
    case BinaryOperatorKind::PlusEqual:
      return "+=";
    default:
      jcc_unimplemented();
  }
}
void BinaryExpr::dump(int indent) const {
  InsertIndent(indent);
  fmt::print("BinaryExpr({}):\n", PrintBinaryOpKind(kind_));
  lhs_->dump(indent + 2);
  rhs_->dump(indent + 2);
}

ArraySubscriptExpr* ArraySubscriptExpr::create(ASTContext& ctx, SourceRange loc,
                                               Expr* lhs, Expr* rhs) {
  void* mem = ctx.Allocate<ArraySubscriptExpr>();
  return new (mem) ArraySubscriptExpr{std::move(loc), lhs, rhs};
}

void ArraySubscriptExpr::dump(int indent) const { jcc_unimplemented(); }

MemberExpr* MemberExpr::create(ASTContext& ctx, SourceRange loc, Stmt* base,
                               Decl* member) {
  void* mem = ctx.Allocate<MemberExpr>();
  return new (mem) MemberExpr{std::move(loc), base, member};
}

void MemberExpr::dump(int indent) const { jcc_unimplemented(); }

DeclRefExpr* DeclRefExpr::Create(ASTContext& ctx, SourceRange loc, Decl* decl) {
  void* mem = ctx.Allocate<DeclRefExpr>();
  return new (mem) DeclRefExpr{std::move(loc), decl};
}

// FIXME: WE don't really need to print that much information.
void DeclRefExpr::dump(int indent) const { decl_->dump(indent); }

ReturnStatement* ReturnStatement::Create(ASTContext& ctx, SourceRange loc,
                                         Expr* return_expr) {
  void* mem = ctx.Allocate<ReturnStatement>();
  return new (mem) ReturnStatement(std::move(loc), return_expr);
}

void ReturnStatement::dump(int indent) const {
  InsertIndent(indent);
  fmt::print("ReturnStatement\n");
  if (return_expr_ != nullptr) {
    return_expr_->dump(indent + 2);
  } else {
    InsertIndent(indent + 2);
    fmt::print("Empty\n");
  }
}

IfStatement* IfStatement::Create(ASTContext& ctx, SourceRange loc,
                                 Expr* condition, Stmt* thenStmt,
                                 Stmt* elseStmt) {
  void* mem = ctx.Allocate<IfStatement>();
  return new (mem) IfStatement(std::move(loc), condition, thenStmt, elseStmt);
}

void IfStatement::dump(int indent) const {
  InsertIndent(indent);
  fmt::print("IfStatement\n");
  condition_->dump(indent + 2);
  // FIXME: they're not always exsits.
  then_stmt_->dump(indent + 2);
  else_stmt_->dump(indent + 2);
}

WhileStatement* WhileStatement::Create(ASTContext& ctx, SourceRange loc,
                                       Expr* condition, Stmt* body) {
  void* mem = ctx.Allocate<WhileStatement>();
  return new (mem) WhileStatement(std::move(loc), condition, body);
}

void WhileStatement::dump(int indent) const {
  InsertIndent(indent);
  fmt::print("WhileStatement\n");
  condition_->dump(indent + 2);
  // FIXME: it's not always exsits.
  body_->dump(indent + 2);
}

DoStatement* DoStatement::Create(ASTContext& ctx, SourceRange loc,
                                 Expr* condition, Stmt* body) {
  void* mem = ctx.Allocate<DoStatement>();
  return new (mem) DoStatement(std::move(loc), condition, body);
}

void DoStatement::dump(int indent) const {
  InsertIndent(indent);
  fmt::print("DoStatement\n");
  // FIXME: it's not always exsits.
  body_->dump(indent + 2);
  condition_->dump(indent + 2);
}

ForStatement* ForStatement::Create(ASTContext& ctx, SourceRange loc, Stmt* init,
                                   Stmt* condition, Stmt* increment,
                                   Stmt* body) {
  void* mem = ctx.Allocate<ForStatement>();
  return new (mem)
      ForStatement(std::move(loc), init, condition, increment, body);
}

void ForStatement::dump(int indent) const {
  InsertIndent(indent);
  fmt::print("ForStatement:\n");

  if (init_ != nullptr) {
    init_->dump(indent + 2);
  }
  if (condition_ != nullptr) {
    condition_->dump(indent + 2);
  }
  if (increment_ != nullptr) {
    increment_->dump(indent + 2);
  }
  if (body_ != nullptr) {
    body_->dump(indent + 2);
  }
}

SwitchStatement* SwitchStatement::Create(ASTContext& ctx, SourceRange loc,
                                         Expr* condition,
                                         CompoundStatement* body) {
  void* mem = ctx.Allocate<SwitchStatement>();
  return new (mem) SwitchStatement(std::move(loc), condition, body);
}

void SwitchStatement::dump(int indent) const {
  InsertIndent(indent);
  fmt::print("SwitchStatement\n");
  condition_->dump(indent + 2);
  body_->dump(indent + 2);
}

CaseStatement* CaseStatement::Create(ASTContext& ctx, SourceRange loc,
                                     Stmt* stmt, std::string value) {
  void* mem = ctx.Allocate<CaseStatement>();
  return new (mem) CaseStatement(std::move(loc), stmt, std::move(value));
}

void CaseStatement::dump(int indent) const {
  InsertIndent(indent);
  fmt::print("CaseStatement\n");
  InsertIndent(indent);
  fmt::print("value: {}\n", GetValue());
  stmt_->dump(indent + 2);
}

DefaultStatement* DefaultStatement::Create(ASTContext& ctx, SourceRange loc,
                                           Stmt* stmt) {
  void* mem = ctx.Allocate<CaseStatement>();
  return new (mem) DefaultStatement(std::move(loc), stmt);
}

void DefaultStatement::dump(int indent) const {
  InsertIndent(indent);
  fmt::print("DefaultStatement\n");
  stmt_->dump(indent + 2);
}

DeclStatement* DeclStatement::Create(ASTContext& ctx, SourceRange loc,
                                     std::vector<Decl*> decls) {
  void* mem = ctx.Allocate<DeclStatement>();
  return new (mem) DeclStatement(std::move(loc), std::move(decls));
}
DeclStatement* DeclStatement::Create(ASTContext& ctx, SourceRange loc,
                                     Decl* decl) {
  void* mem = ctx.Allocate<DeclStatement>();
  return new (mem) DeclStatement(std::move(loc), decl);
}

void DeclStatement::dump(int indent) const {
  for (const auto* decl : decls_) {
    decl->dump(indent);
  }
}

CompoundStatement* CompoundStatement::Create(ASTContext& ctx, SourceRange loc) {
  void* mem = ctx.Allocate<CompoundStatement>();
  return new (mem) CompoundStatement(std::move(loc));
}

void CompoundStatement::dump(int indent) const {
  InsertIndent(indent);
  fmt::print("CompoundStatement\n");
  for (const auto* stmt : stmts_) {
    stmt->dump(indent + 2);
  }
}

ExprStatement* ExprStatement::Create(ASTContext& ctx, SourceRange loc,
                                     Expr* expr) {
  void* mem = ctx.Allocate<ExprStatement>();
  return new (mem) ExprStatement(std::move(loc), expr);
}

void ExprStatement::dump(int indent) const {
  InsertIndent(indent);
  fmt::print("ExprStatement\n");
  expr_->dump(indent + 2);
}

BreakStatement* BreakStatement::Create(ASTContext& ctx, SourceRange loc,
                                       SourceRange break_loc) {
  void* mem = ctx.Allocate<BreakStatement>();
  return new (mem) BreakStatement(std::move(loc), std::move(break_loc));
}

void BreakStatement::dump(int indent) const {
  InsertIndent(indent);
  fmt::print("BreakStatement\n");
}

ContinueStatement* ContinueStatement::Create(ASTContext& ctx, SourceRange loc,
                                             SourceRange continue_loc) {
  void* mem = ctx.Allocate<ContinueStatement>();
  return new (mem) ContinueStatement(std::move(loc), std::move(continue_loc));
}

void ContinueStatement::dump(int indent) const {
  InsertIndent(indent);
  fmt::print("ContinueStatement\n");
}

}  // namespace jcc
