#include "jcc/codegen.h"

namespace jcc {

void CodeGen::Emit() {
  Write(" .global main\n");
  Write(" .main:\n");
  Write("  ret\n");
}

void CodeGen::EmitVarDecl(VarDecl& decl) {}

void CodeGen::EmitFunctionDecl(FunctionDecl& decl) {}

void CodeGen::EmitRecordDecl(RecordDecl& decl) {}

void CodeGen::EmitIfStatement(IfStatement& stmt) {}

void CodeGen::EmitWhileStatement(WhileStatement& stmt) {}

void CodeGen::EmitReturnStatement(ReturnStatement& stmt) {}

void CodeGen::EmitDeclStatement(DeclStatement& stmt) {}

void CodeGen::EmitExprStatement(ExprStatement& stmt) {}

void CodeGen::EmitCompoundStatement(CompoundStatement& stmt) {}

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
