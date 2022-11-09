#pragma once

#include <fmt/format.h>

#include <cassert>
#include <cstdlib>
#include <string>
#include <string_view>

#include "fmt/core.h"
#include "jcc/stmt.h"

namespace jcc {

class VarDecl;
class FunctionDecl;
class RecordDecl;
class IfStatement;
class WhileStatement;
class DoStatement;
class ForStatement;
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
  bool is_valid_ = false;
  static constexpr const char* mode = "w+";

 public:
  explicit File(std::string_view file_name) {
    data_ = std::fopen(file_name.data(), mode);
    is_valid_ = (data_ != nullptr);
  }
  ~File() {
    if (is_valid_) {
      std::fclose(data_);
    }
  }

  template <typename S, typename... Args>
  void Write(const S& format, Args... args) {
    assert(is_valid_ && "can't write to invalid file!");
    fmt::vprint(data_, format, fmt::make_format_args(args...));
  }
};

class CodeGen {
  File file_;

 public:
  explicit CodeGen(const std::string& file_name);

  void EmitVarDecl(VarDecl& decl);
  void EmitFunctionDecl(FunctionDecl& decl);
  void EmitRecordDecl(RecordDecl& decl);

  void EmitIfStatement(IfStatement& stmt);
  void EmitWhileStatement(WhileStatement& stmt);
  void EmitDoStatement(DoStatement& stmt);
  void EmitForStatement(ForStatement& stmt);
  void EmitReturnStatement(ReturnStatement& stmt);
  void EmitBreakStatement(BreakStatement& stmt);
  void EmitContinueStatement(ContinueStatement& stmt);
  void EmitDeclStatement(DeclStatement& stmt);
  void EmitExprStatement(ExprStatement& stmt);
  void EmitCompoundStatement(CompoundStatement& stmt);

  void EmitStringLiteral(StringLiteral& expr);
  void EmitCharacterLiteral(CharacterLiteral& expr);
  void EmitIntergerLiteral(IntergerLiteral& expr);
  void EmitFloatingLiteral(FloatingLiteral& expr);
  void EmitCallExpr(CallExpr& expr);
  void EmitUnaryExpr(UnaryExpr& expr);
  void EmitBinaryExpr(BinaryExpr& expr);
  void EmitArraySubscriptExpr(ArraySubscriptExpr& expr);
  void EmitMemberExpr(MemberExpr& expr);
  void EmitDeclRefExpr(DeclRefExpr& expr);

 private:
  template <typename S, typename... Args>
  void Write(const S& format, Args&&... args) {
    file_.Write(format, std::forward<Args>(args)...);
    file_.Write("\n");
  }
  void Push() {
    Write(" push %rax\n");
    StackDepthTracker::Push();
  }
  void Pop(std::string_view arg) {
    Write(" pop {}\n", arg);
    StackDepthTracker::Pop();
  }

  void Init();
};
}  // namespace jcc
