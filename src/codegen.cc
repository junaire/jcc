#include "jcc/codegen.h"

#include <cstddef>

#include "jcc/decl.h"
#include "jcc/expr.h"
#include "jcc/stmt.h"

namespace jcc {

CodeGen::CodeGen(std::string_view file_name) : file_(file_name) { Init(); }

void CodeGen::Init() {
	Write(".intel_syntax noprefix");
}

void CodeGen::EmitVarDecl(VarDecl& decl) {}

void CodeGen::EmitFunctionDecl(FunctionDecl& decl) {
  Write("{}:", decl.GetName());
  Write("  push rbp");
  Write("  mov rbp, rsp");
  decl.GetBody()->GenCode(*this);
  Write("  pop rbp");
  Write("  ret");
}

void CodeGen::EmitRecordDecl(RecordDecl& decl) {}

void CodeGen::EmitIfStatement(IfStatement& stmt) {}

void CodeGen::EmitWhileStatement(WhileStatement& stmt) {}

void CodeGen::EmitReturnStatement(ReturnStatement& stmt) {
  auto value = stmt.GetReturn()->AsExpr<IntergerLiteral>()->GetValue();
  Write("  mov eax, {}", value);
}

void CodeGen::EmitDeclStatement(DeclStatement& stmt) {}

void CodeGen::EmitExprStatement(ExprStatement& stmt) {}

void CodeGen::EmitCompoundStatement(CompoundStatement& stmt) {
  for (std::size_t idx = 0; idx < stmt.GetSize(); idx++) {
    stmt.GetStmt(idx)->GenCode(*this);
  }
}

void CodeGen::EmitStringLiteral(StringLiteral& expr) {}

void CodeGen::EmitCharacterLiteral(CharacterLiteral& expr) {}

void CodeGen::EmitIntergerLiteral(IntergerLiteral& expr) {}

void CodeGen::EmitFloatingLiteral(FloatingLiteral& expr) {}

void CodeGen::EmitCallExpr(CallExpr& expr) {}

void CodeGen::EmitUnaryExpr(UnaryExpr& expr) {}

void CodeGen::EmitBinaryExpr(BinaryExpr& expr) {}

void CodeGen::EmitArraySubscriptExpr(ArraySubscriptExpr& expr) {}

void CodeGen::EmitMemberExpr(MemberExpr& expr) {}

void CodeGen::EmitDeclRefExpr(DeclRefExpr& expr) {}
}  // namespace jcc
