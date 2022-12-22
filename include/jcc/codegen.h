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
std::string GenerateAssembly(const std::string& file_name,
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

class File {
  std::FILE* data_ = nullptr;
  static constexpr const char* mode = "w+";
  std::string file_name_;

 public:
  explicit File(std::string file_name) : file_name_(std::move(file_name)) {
    data_ = std::fopen(file_name_.data(), mode);
  }

  ~File() {
    if (data_ != nullptr) {
      std::fclose(data_);
    }
  }

  [[nodiscard]] std::string GetName() const { return file_name_; }

  template <typename S, typename... Args>
  void Write(const S& format, Args... args) {
    assert(data_ && "can't write to invalid file!");
    fmt::vprint(data_, format, fmt::make_format_args(args...));
  }
};

#define EMITDECL(Node) void Emit##Node(Node& decl);
#define EMITSTMT(Node) void Emit##Node(Node& stmt);
#define EMITEXPR(Node) void Emit##Node(Node& expr);

class CodeGen {
  File file_;
  CodeGenContext ctx;

 public:
  explicit CodeGen(const std::string& file_name);

  File GetFile() { return file_; }

  [[nodiscard]] std::string GetFileName() const { return file_.GetName(); }

  void AssignLocalOffsets(const std::vector<Decl*>& decls);

  std::optional<int> GetLocalOffset(Decl* decl) {
    auto iter = ctx.offsets.find(decl);
    if (iter != ctx.offsets.end()) {
      return iter->second;
    }
    return std::nullopt;
  }

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
    file_.Write(format, std::forward<Args>(args)...);
  }

  template <typename S, typename... Args>
  void Writeln(const S& format, Args&&... args) {
    Write(format, std::forward<Args>(args)...);
    file_.Write("\n");
  }

  void Push() {
    Writeln("  push rax");
    StackDepthTracker::Push();
  }
  void Pop(std::string_view arg) {
    Writeln("  pop {}", arg);
    StackDepthTracker::Pop();
  }

  void CompZero(const Type& type);

  class EmitFunctionRAII {
    CodeGen& gen_;

   public:
    explicit EmitFunctionRAII(CodeGen& gen) : gen_(gen) {
      gen_.Writeln("  push rbp");
      gen_.Writeln("  mov rbp, rsp");
    }
    ~EmitFunctionRAII() {
      gen_.Writeln("  mov rsp, rbp");
      gen_.Writeln("  pop rbp");
      gen_.Writeln("  ret");
    }
  };
};
}  // namespace jcc
