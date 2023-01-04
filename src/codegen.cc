#include "jcc/codegen.h"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <fstream>
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

static constexpr std::string_view arg_reg8[] = {"%dil", "%sil", "%dl",
                                                "%cl",  "%r8b", "%r9b"};
static constexpr std::string_view arg_reg16[] = {"%di", "%si",  "%dx",
                                                 "%cx", "%r8w", "%r9w"};
static constexpr std::string_view arg_reg32[] = {"%edi", "%esi", "%edx",
                                                 "%ecx", "%r8d", "%r9d"};
static constexpr std::string_view arg_reg64[] = {"%rdi", "%rsi", "%rdx",
                                                 "%rcx", "%r8",  "%r9"};

// The max number of registers we can use when call a function.
static constexpr size_t gp_max = 6;

static int AlignTo(int n, int align) { return (n + align - 1) / align * align; }

static void AssignLocalOffsets(const std::vector<Decl*>& decls) {
  for (Decl* decl : decls) {
    if (auto* func = decl->As<FunctionDecl>(); func != nullptr) {
      // If a function has many parameters, some parameters are
      // inevitably passed by stack rather than by register.
      // The first passed-by-stack parameter resides at RBP+16.
      int top = 16;
      int bottom = 0;
      size_t gp = 0;
      size_t fp = 0;

      // Assign offsets to pass-by-stack parameters.
      for (size_t idx = 0; idx < func->GetParamNum(); ++idx) {
        VarDecl* param = func->GetParam(idx);
        Type* param_type = param->GetType();
        switch (param_type->GetKind()) {
          case TypeKind::Struct:
          case TypeKind::Union:
            jcc_unimplemented();
            break;
          case TypeKind::Float:
          case TypeKind::Double:
            jcc_unimplemented();
            break;
          case TypeKind::Ldouble:
            jcc_unimplemented();
            break;
          default: {
            if (gp++ < gp_max) {
              continue;
            }
          }
        }
        top = AlignTo(top, 8);
        param->SetOffset(top);
        top += param_type->GetSize();
      }

      auto set_offset = [&](VarDecl* var) {
        Type* type = var->GetType();
        // AMD64 System V ABI has a special alignment rule for an array of
        // length at least 16 bytes. We need to align such array to at
        // least 16-byte boundaries. See p.14 of
        // https://github.com/hjl-tools/x86-psABI/wiki/x86-64-psABI-draft.pdf.
        size_t align =
            (type->Is<TypeKind::Array>() && type->GetSize() >= 16)
                ? std::max(static_cast<size_t>(16), type->GetAlignment())
                : type->GetAlignment();

        bottom += type->GetSize();
        bottom = AlignTo(bottom, align);
        var->SetOffset(-bottom);
      };

      for (const auto& local : func->GetLocals()) {
        // Is this check really needed?
        if (auto* var_decl = local->As<VarDecl>(); var_decl != nullptr) {
          set_offset(var_decl);
        }
      }

      // Assign offsets to pass-by-register parameters.
      for (size_t idx = 0; idx < func->GetParamNum(); ++idx) {
        VarDecl* param = func->GetParam(idx);
        if (param->GetOffset().has_value()) {
          continue;
        }
        set_offset(param);
      }
      func->SetStackSize(AlignTo(bottom, 16));
    }
  }
}

void GenerateAssembly(const std::string& file_name,
                      const std::vector<jcc::Decl*>& decls) {
  CodeGen generator(file_name);
  AssignLocalOffsets(decls);
  for (Decl* decl : decls) {
    decl->GenCode(generator);
  }
}

CodeGen::CodeGen(const std::string& file_name)
    : name_(CreateAsmFileName(file_name)) {
  EmitSectionRAII section_guard(*this, Section::Header);
  Writeln(R"(  .file "{}")", file_name);
}

CodeGen::~CodeGen() {
  std::fstream out(name_, std::ios::out | std::ios::trunc);
  out << header_;
  out << data_;
  out << text_;
  out.close();
}

void CodeGen::Store(const Type& type) {
  Pop("%rdi");
  switch (type.GetKind()) {
    case TypeKind::Int:
      Writeln("  mov %eax, (%rdi)");
      break;
    default:
      Writeln("  mov %rax, (%rdi)");
      // jcc_unimplemented();
  }
}

void CodeGen::Load(const Type& type) {
  switch (type.GetSize()) {
    case 4:
      Writeln("  movsxd (%rax), %rax");
      break;
    default:
      Writeln("  mov (%rax), %rax");
  }
}

void CodeGen::Assign(Decl& decl, Stmt* init) {
  if (init == nullptr) {
    return;
  }
  // TODO(Jun): Maybe we need a flag to indicate whether this is a local decl or
  // not?
  if (std::optional<int> offset = decl.GetOffset()) {
    assert(!ctx.cur_func_name.empty() &&
           "We're not inside a funtion? You sure it's a local decl?");

    Writeln("  lea {}(%rbp), %rax", *offset);
    Push();
    init->GenCode(*this);
    Store(*decl.GetType());
  } else {
    jcc_unimplemented();
  }
}

// So we're doing things like:
//   1. rax = &decl, we have allocate enough space for it in the stack.
//   2. store rax on the top of the stack.
//   3. rax = real value like `42`.
//   4. pop the address of the decl and assign it to rdi.
//   5. *rdi = rax, which is the real value.
void CodeGen::EmitVarDecl(VarDecl& decl) { Assign(decl, decl.GetInit()); }

void CodeGen::StoreArgs(FunctionDecl& func) {
  for (size_t i = 0; i < func.GetParamNum(); ++i) {
    VarDecl* arg = func.GetParam(i);
    switch (arg->GetType()->GetSize()) {
      case 4: {
        if (std::optional<int> offset = arg->GetOffset()) {
          Writeln("  mov {}, {}(%rbp)", arg_reg32[i], *offset);
          break;
        }
        jcc_unimplemented();
      }
      default:
        jcc_unimplemented();
    }
  }
}

void CodeGen::EmitFunctionDecl(FunctionDecl& decl) {
  // Just a declaration, no need to emit code.
  if (!decl.HasDefinition()) {
    return;
  }

  // TODO(Jun): Keep informations like `static`, `extern` and etc.

  ctx.cur_func_name = decl.GetName();
  Writeln("  .globl {}", decl.GetName());
  Writeln("  .type {}, @function", decl.GetName());
  Writeln("  .text");
  Writeln("{}:", decl.GetName());

  EmitFunctionRAII emit_func_guard(*this);

  // Allocate stack for the function.
  // FIXME: these're just arbitrary numbers.
  Writeln("  sub ${}, %rsp", decl.GetStackSize());
  // Handle varidic function.

  // Save passed by regisiter arguments.
  StoreArgs(decl);

  // Emit code for body.
  decl.GetBody()->GenCode(*this);

  // Section for ret.
  if (decl.IsMain()) {
    Writeln("  mov $0, %rax");
  }
  Writeln(".L.return.{}:", decl.GetName());
}

void CodeGen::EmitRecordDecl(RecordDecl& decl) {}

void CodeGen::CompZero(const Type& type) {
  if (type.IsInteger()) {
    const char* instr = type.GetSize() <= 4 ? "%eax" : "%rax";
    Writeln("  cmp $0, {}", instr);
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
  Writeln(".L.else.{}:", section_cnt);
  if (auto* else_stmt = stmt.GetElse()) {
    else_stmt->GenCode(*this);
  }
  Writeln(".L.end.{}:", section_cnt);
}

void CodeGen::EmitWhileStatement(WhileStatement& stmt) {
  int64_t section_cnt = Counter();
  Writeln(".L.begin.{}:", section_cnt);
  if (auto* cond = stmt.GetCondition()) {
    cond->GenCode(*this);
    CompZero(*cond->GetType());
  }
  Writeln("  je .L.body.{}", section_cnt);
  stmt.GetBody()->GenCode(*this);
  Writeln("  jmp .L.begin.{}", section_cnt);
  Writeln(".L.body.{}:", section_cnt);
}

void CodeGen::EmitDoStatement(DoStatement& stmt) {
  int64_t section_cnt = Counter();
  Writeln(".L.begin.{}:", section_cnt);
  stmt.GetBody()->GenCode(*this);
  stmt.GetCondition()->GenCode(*this);
  Writeln("  jne .L.begin.{}", section_cnt);
}

// TODO(Jun): Support continue and break statements.
void CodeGen::EmitForStatement(ForStatement& stmt) {
  int64_t section_cnt = Counter();
  if (auto* init = stmt.GetInit()) {
    init->GenCode(*this);
  }
  Writeln(".L.begin.{}:", section_cnt);
  if (Stmt* condition = stmt.GetCondition()) {
    condition->GenCode(*this);
    // FIXME: WE should really reevaluate it the relationship between stmt and
    // expr.
    if (auto* cond_expr = condition->As<ExprStatement>();
        cond_expr != nullptr) {
      CompZero(*cond_expr->GetExpr()->GetType());
    } else {
      jcc_unreachable("Condition should has a expr!");
    }
    Writeln("  je .L.end.{}", section_cnt);
  }
  stmt.GetBody()->GenCode(*this);
  if (Stmt* inc = stmt.GetIncrement()) {
    inc->GenCode(*this);
  }
  Writeln("  jmp .L.begin.{}", section_cnt);
  Writeln(".L.end.{}:", section_cnt);
}

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

void CodeGen::EmitStringLiteral(StringLiteral& expr) {
  // Include the null terminator.
  size_t size = expr.GetValue().size() + 1;
  std::string name = fmt::format(".L..{}", Counter());
  std::string value = expr.GetValue();
  {
    EmitSectionRAII section_guard(*this, Section::Data);

    Writeln("  .local {}", name);
    // TODO(Jun): Support .tdata
    Writeln("  .data");
    Writeln("  .type {}, @object", name);
    Writeln("  .size {}, {}", name, size);
    // FIXME: What's the type of the StringLiteral?
    Writeln("  .align {}", 1);
    Writeln("{}:", name);
    for (size_t i = 0; i < size; ++i) {
      Writeln("  .byte {}", static_cast<int>(value[i]));
    }
  }
  Writeln("  lea {}(%rip), %rax", name);
}

void CodeGen::EmitCharacterLiteral(CharacterLiteral& expr) {
  assert(expr.GetValue().size() == 1 && "Not a character?");
  Writeln("  mov {}, %rax", static_cast<int>(expr.GetValue()[0]));
}

void CodeGen::EmitIntergerLiteral(IntergerLiteral& expr) {
  Writeln("  mov ${}, %rax", expr.GetValue());
}

void CodeGen::EmitFloatingLiteral(FloatingLiteral& expr) {}

void CodeGen::LoadArgs(FunctionDecl& func) {
  size_t stack = 0;
  size_t gp = 0;
  size_t fp = 0;

  // TODO(Jun): What if the return type is a large struct or union?

  for (size_t i = 0; i < func.GetParamNum(); ++i) {
    VarDecl* arg = func.GetParam(i);
    switch (arg->GetType()->GetKind()) {
      case TypeKind::Struct:
      case TypeKind::Union:
      case TypeKind::Float:
        jcc_unimplemented();
      default: {
        if (gp++ >= gp_max) {
          stack++;
        }
      }
    }
  }
}
void CodeGen::EmitCallExpr(CallExpr& expr) {
  for (size_t i = 0; i < expr.GetArgNum(); ++i) {
    expr.GetArg(i)->GenCode(*this);
    Push();
  }
  auto* func =
      expr.GetCallee()->As<DeclRefExpr>()->GetRefDecl()->As<FunctionDecl>();
  if (func->HasDefinition()) {
    Writeln("  lea {}(%rip), %rax", func->GetName());
  } else {
    Writeln("  mov {}@GOTPCREL(%rip), %rax", func->GetName());
  }
  LoadArgs(*func);

  Writeln("  mov %rax, %r10");
  Writeln("  mov $0, %rax");
  Writeln("  call *%r10");
  Writeln("  add $0, %rsp");
}

void CodeGen::EmitUnaryExpr(UnaryExpr& expr) {
  expr.GetValue()->GenCode(*this);
  switch (expr.getKind()) {
    case UnaryOperatorKind::PostIncrement: {
      if (auto* ref_expr = expr.GetValue()->As<DeclRefExpr>();
          ref_expr->GetRefDecl()->GetType()->GetSize() == 4) {
        Writeln("  addl $1, {}(%rbp)", *ref_expr->GetRefDecl()->GetOffset());
      } else {
        jcc_unimplemented();
      }
      break;
    }
    default:
      jcc_unimplemented();
  }
}

void CodeGen::EmitBinaryExpr(BinaryExpr& expr) {
  using enum BinaryOperatorKind;
  switch (expr.GetKind()) {
    case Greater:
    case Less: {
      // Here we share the same code for `>` and `<` by exchange the order.
      Expr* lhs = expr.GetKind() == Greater ? expr.GetLhs() : expr.GetRhs();
      Expr* rhs = expr.GetKind() == Greater ? expr.GetRhs() : expr.GetLhs();
      // Store lhs and rhs to rdi and rax respectively.
      lhs->GenCode(*this);
      Push();
      rhs->GenCode(*this);
      Pop("%rdi");
      // FIXME: Register size!
      assert(expr.GetLhs()->GetType()->GetSize() == 4);
      Writeln("  cmp {}, {}", "%edi", "%eax");
      const char* instr =
          expr.GetLhs()->GetType()->IsUnsigned() ? "setb" : "setl";
      Writeln("  {} %al", instr);
      Writeln("  movzb %al, %rax");
      break;
    }
    case Equal: {
      // Can't invoke `EmitDeclRefExpr` as it will call `Load`
      auto* ref_expr = expr.GetLhs()->As<DeclRefExpr>();
      assert(ref_expr != nullptr &&
             "The Lhs of the BinaryExpr is not a DeclRefExpr?");
      Assign(*ref_expr->GetRefDecl(), expr.GetRhs());
      break;
    }
    case PlusEqual: {
      jcc_unimplemented();
    }
    case Plus: {
      expr.GetLhs()->GenCode(*this);
      Push();
      expr.GetRhs()->GenCode(*this);
      Pop("%rdi");
      Writeln("  add {}, {}", "%edi", "%eax");
      break;
    }
    default:
      jcc_unimplemented();
  }
}

void CodeGen::EmitArraySubscriptExpr(ArraySubscriptExpr& expr) {}

void CodeGen::EmitMemberExpr(MemberExpr& expr) {}

void CodeGen::EmitDeclRefExpr(DeclRefExpr& expr) {
  if (std::optional<int> offset = expr.GetRefDecl()->GetOffset()) {
    Writeln("  lea {}(%rbp), %rax", *offset);

    // TODO(Jun): Implement cases when we have char or double types and etc.
    // const char* instr = type->IsUnsigned() ? "movz" : "mos";
    Load(*expr.GetRefDecl()->GetType());
  } else {
    jcc_unreachable("Can DeclRefExpr store a global decl?");
  }
}
}  // namespace jcc
