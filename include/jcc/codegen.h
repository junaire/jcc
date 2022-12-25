#pragma once

#include <fmt/format.h>

#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <map>
#include <optional>
#include <string>
#include <string_view>

#include "fmt/core.h"
#include "jcc/stmt.h"

namespace jcc {

class Type;

class VarDecl;
class FunctionDecl;
class RecordDecl;
class IfStatement;
class WhileStatement;
class DoStatement;
class ForStatement;
class SwitchStatement;
class CaseStatement;
class DefaultStatement;
class ReturnStatement;
class DeclStatement;
class ExprStatement;
class CompoundStatement;
class StringLiteral;
class CharacterLiteral;
class IntergerLiteral;
class FloatingLiteral;
class CallExpr;
class UnaryExpr;
class BinaryExpr;
class ArraySubscriptExpr;
class MemberExpr;
class DeclRefExpr;

struct CodeGenContext {
  std::string cur_func_name;
  std::map<Decl*, size_t> offsets;
  std::map<FunctionDecl*, size_t> func_stack_size;
};

// Entry point for generate assembly code.
void GenerateAssembly(const std::string& file_name,
                      const std::vector<jcc::Decl*>& decls);

class StackDepthTracker {
 public:
  static int& Get() {
    static int depth = 0;
    return depth;
  }
  static void Push() {
    int& depth = Get();
    depth++;
  }
  static void Pop() {
    int& depth = Get();
    depth--;
  }
};

#define EMITDECL(Node) void Emit##Node(Node& decl);
#define EMITSTMT(Node) void Emit##Node(Node& stmt);
#define EMITEXPR(Node) void Emit##Node(Node& expr);

class CodeGen {
  friend class EmitDataSectionRAII;

 public:
  enum class Section { Header, Data, Text };

  explicit CodeGen(const std::string& file_name);

  ~CodeGen();

  void AssignLocalOffsets(const std::vector<Decl*>& decls);

  EMITDECL(VarDecl)
  EMITDECL(FunctionDecl)
  EMITDECL(RecordDecl)

  EMITSTMT(IfStatement)
  EMITSTMT(WhileStatement)
  EMITSTMT(DoStatement)
  EMITSTMT(ForStatement)
  EMITSTMT(SwitchStatement)
  EMITSTMT(CaseStatement)
  EMITSTMT(DefaultStatement)
  EMITSTMT(ReturnStatement)
  EMITSTMT(BreakStatement)
  EMITSTMT(ContinueStatement)
  EMITSTMT(DeclStatement)
  EMITSTMT(ExprStatement)
  EMITSTMT(CompoundStatement)

  EMITEXPR(StringLiteral);
  EMITEXPR(CharacterLiteral);
  EMITEXPR(IntergerLiteral);
  EMITEXPR(FloatingLiteral);
  EMITEXPR(CallExpr);
  EMITEXPR(UnaryExpr);
  EMITEXPR(BinaryExpr);
  EMITEXPR(ArraySubscriptExpr);
  EMITEXPR(MemberExpr);
  EMITEXPR(DeclRefExpr);

 private:
  template <typename S, typename... Args>
  void Write(const S& format, Args&&... args) {
    if (IsDataSection()) {
      data_ += fmt::format(fmt::runtime(format), std::forward<Args>(args)...);
      return;
    }
    if (IsHeaderSection()) {
      header_ += fmt::format(fmt::runtime(format), std::forward<Args>(args)...);
      return;
    }
    assert(section_ == Section::Text);
    text_ += fmt::format(fmt::runtime(format), std::forward<Args>(args)...);
  }

  template <typename S, typename... Args>
  void Writeln(const S& format, Args&&... args) {
    Write(format, std::forward<Args>(args)...);
    Write("\n");
  }

  void Push() {
    Writeln("  push %rax");
    StackDepthTracker::Push();
  }
  void Pop(std::string_view arg) {
    Writeln("  pop {}", arg);
    StackDepthTracker::Pop();
  }

  void Store(const Type& type);

  // When we load a char or a short value to a register, we always
  // extend them to the size of int, so we can assume the lower half of
  // a register always contains a valid value. The upper half of a
  // register for char, short and int may contain garbage. When we load
  // a long value to a register, it simply occupies the entire register.
  void Load(const Type& type);

  // Consider cases below:
  //   1. int x = 42;
  //   2. y = 42;
  // case 1 is a VarDecl and case 2 is a BinaryExpr,
  // though they are different AST nodes, they generally
  // share almost same codegen process.
  void Assign(Decl& decl, Stmt* init);

  std::optional<int> GetLocalOffset(Decl* decl) {
    auto iter = ctx.offsets.find(decl);
    if (iter != ctx.offsets.end()) {
      return iter->second;
    }
    return std::nullopt;
  }
  void CompZero(const Type& type);

  [[nodiscard]] bool IsDataSection() const { return section_ == Section::Data; }

  [[nodiscard]] bool IsHeaderSection() const {
    return section_ == Section::Header;
  }

  class EmitFunctionRAII {
    CodeGen& gen_;

   public:
    explicit EmitFunctionRAII(CodeGen& gen) : gen_(gen) {
      gen_.Writeln("  push %rbp");
      gen_.Writeln("  mov %rsp, %rbp");
    }
    ~EmitFunctionRAII() {
      gen_.Writeln("  mov %rbp, %rsp");
      gen_.Writeln("  pop %rbp");
      gen_.Writeln("  ret");
    }
  };

  class EmitSectionRAII {
    CodeGen& gen_;

   public:
    explicit EmitSectionRAII(CodeGen& gen, Section section) : gen_(gen) {
      gen_.section_ = section;
    }
    ~EmitSectionRAII() { gen_.section_ = Section::Text; }
  };

  std::string name_;

  // Text section.
  std::string text_;
  // Data section.
  std::string data_;

  // This is onlt used for program header annotation.
  std::string header_;

  CodeGenContext ctx;

  Section section_ = Section::Text;
};
}  // namespace jcc
