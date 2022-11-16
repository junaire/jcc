#include "jcc/codegen.h"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <string_view>

#include "jcc/common.h"
#include "jcc/decl.h"
#include "jcc/expr.h"
#include "jcc/stmt.h"
#include "jcc/type.h"

// Turn foo.c => foo.s
static std::string CreateAsmFileName(const std::string& name) {
  assert(name.ends_with(".c") &&
         "Can't generate asm code for non C source code!");
  std::string asm_file = name.substr(0, name.size() - 2);
  return asm_file + ".s";
}

// Helper function for section naming.
static int64_t Counter() {
  static int64_t cnt = 1;
  return cnt++;
}

namespace jcc {

std::string GenerateAssembly(const std::string& file_name,
                             const std::vector<jcc::Decl*>& decls) {
  jcc::CodeGen generator(file_name);
  for (jcc::Decl* decl : decls) {
    decl->GenCode(generator);
  }
  return generator.GetFileName();
}

CodeGen::CodeGen(const std::string& file_name)
    : file_(CreateAsmFileName(file_name)) {
  Writeln(R"(  .file "{}")", file_name);
  Writeln("  .intel_syntax noprefix");
  Writeln("  .text");
}

void CodeGen::EmitVarDecl(VarDecl& decl) {
  if (decl.GetType()->IsInteger()) {
    Writeln("  lea rax, -{}[rbp]", decl.GetType()->GetSize());
    Push();
    decl.GetInit()->GenCode(*this);
    Pop("rdi");
    Writeln("  mov [rdi], eax");
  }
}

void CodeGen::EmitFunctionDecl(FunctionDecl& decl) {
  ctx.cur_func_name = decl.GetName();
  Writeln("  .global {}", decl.GetName());
  Writeln("  .type {}, @function", decl.GetName());
  Writeln("{}:", decl.GetName());

  EmitFunctionRAII emit_func_guard(*this);

  // Allocate stack for the function.
  // FIXME: these're just arbitrary numbers.
  Writeln("  sub rsp, {}", 160);
  Writeln("  mov {}[rbp], rsp", -16);
  // Handle varidic function.
  // Save passed by regisiter arguments.

  // Emit code for body.
  decl.GetBody()->GenCode(*this);

  // Section for ret.
  if (decl.IsMain()) {
    Writeln("  mov rax, 0");
  }
  Writeln(".L.return.{}:", decl.GetName());
}

void CodeGen::EmitRecordDecl(RecordDecl& decl) {}

void CodeGen::CompZero(const Type& type) {
  if (type.IsInteger()) {
    Writeln("  cmp rax, 0");
    return;
  }
  jcc_unimplemented();
}

void CodeGen::EmitIfStatement(IfStatement& stmt) {
  int64_t section_cnt = Counter();
  stmt.GetCondition()->GenCode(*this);
  CompZero(*stmt.GetCondition()->GetType());
  Writeln("  je  .L.else.{}", section_cnt);
  stmt.GetThen()->GenCode(*this);
  Writeln("  jmp .L.end.{}", section_cnt);
  Writeln(".L.else.{}", section_cnt);
  if (auto* else_stmt = stmt.GetElse()) {
    else_stmt->GenCode(*this);
  }
  Writeln(".L.end.{}", section_cnt);
}

void CodeGen::EmitWhileStatement(WhileStatement& stmt) {}

void CodeGen::EmitDoStatement(DoStatement& stmt) {}

void CodeGen::EmitForStatement(ForStatement& stmt) {}

void CodeGen::EmitSwitchStatement(SwitchStatement& stmt) {}

void CodeGen::EmitCaseStatement(CaseStatement& stmt) {}

void CodeGen::EmitDefaultStatement(DefaultStatement& stmt) {}

void CodeGen::EmitReturnStatement(ReturnStatement& stmt) {
  if (auto* return_expr = stmt.GetReturn()) {
    return_expr->GenCode(*this);
    if (!return_expr->GetType()->IsInteger()) {
      jcc_unimplemented();
    }
    Writeln("  jmp .L.return.{}", ctx.cur_func_name);
  }
}

void CodeGen::EmitBreakStatement(BreakStatement& stmt) {}

void CodeGen::EmitContinueStatement(ContinueStatement& stmt) {}

void CodeGen::EmitDeclStatement(DeclStatement& stmt) {
  for (auto* decl : stmt.GetDecls()) {
    decl->GenCode(*this);
  }
}

void CodeGen::EmitExprStatement(ExprStatement& stmt) {
  stmt.GetExpr()->GenCode(*this);
}

void CodeGen::EmitCompoundStatement(CompoundStatement& stmt) {
  for (std::size_t idx = 0; idx < stmt.GetSize(); idx++) {
    stmt.GetStmt(idx)->GenCode(*this);
  }
}

void CodeGen::EmitStringLiteral(StringLiteral& expr) {}

void CodeGen::EmitCharacterLiteral(CharacterLiteral& expr) {}

void CodeGen::EmitIntergerLiteral(IntergerLiteral& expr) {
  Writeln("  mov rax, {}", expr.GetValue());
}

void CodeGen::EmitFloatingLiteral(FloatingLiteral& expr) {}

void CodeGen::EmitCallExpr(CallExpr& expr) {}

void CodeGen::EmitUnaryExpr(UnaryExpr& expr) {}

void CodeGen::EmitBinaryExpr(BinaryExpr& expr) {}

void CodeGen::EmitArraySubscriptExpr(ArraySubscriptExpr& expr) {}

void CodeGen::EmitMemberExpr(MemberExpr& expr) {}

void CodeGen::EmitDeclRefExpr(DeclRefExpr& expr) {
  // FIXME: this is totally hot garbage and not working,
  // we need to know the offset of every locals so we can load them.
  Writeln("  lea rax, {}[rbp]", -4);
  // FIXME: this is the cast.
  Writeln("  movsxd rax, [rax]");
}
}  // namespace jcc
