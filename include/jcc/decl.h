#pragma once

#include <cassert>
#include <string>
#include <string_view>
#include <vector>

#include "jcc/ast_node.h"

namespace jcc {
class Type;
class Stmt;
class Expr;
class ASTContext;

class Decl : public ASTNode {
  std::string name_;
  Type* type_ = nullptr;

 protected:
  explicit Decl(SourceRange loc, std::string name, Type* type)
      : ASTNode(std::move(loc)), name_(std::move(name)), type_(type) {}

 public:
  [[nodiscard]] std::string GetName() const { return name_; }

  Type* GetType() {
    assert(type_ != nullptr);
    return type_;
  }

  ~Decl() override;
};

class VarDecl : public Decl {
  Expr* init_ = nullptr;

  VarDecl(SourceRange loc, Expr* init, Type* type, std::string name)
      : Decl(std::move(loc), std::move(name), type), init_(init) {}

 public:
  static VarDecl* Create(ASTContext& ctx, SourceRange loc, Expr* init,
                         Type* type, std::string name);

  Expr* GetInit() { return init_; }

  void SetInit(Expr* init) { init_ = init; }

  [[nodiscard]] bool IsDefinition() const { return init_ == nullptr; }

  void dump(int indent) const override;
  void GenCode(CodeGen& gen) override;
};

class FunctionDecl : public Decl {
  std::vector<VarDecl*> args_;
  // This makes it much easier to assign offsets for them.
  std::vector<Decl*> locals_;
  Type* return_type_;
  Stmt* body_ = nullptr;

  FunctionDecl(SourceRange loc, std::string name, Type* type, Type* return_type)
      : Decl(std::move(loc), std::move(name), type),
        return_type_(return_type) {}

  FunctionDecl(SourceRange loc, std::string name, std::vector<VarDecl*> args,
               Type* type, Type* return_type, Stmt* body)
      : Decl(std::move(loc), std::move(name), type),
        args_(std::move(args)),
        return_type_(return_type),
        body_(body) {}

 public:
  static FunctionDecl* Create(ASTContext& ctx, SourceRange loc,
                              const std::string& name, Type* type,
                              Type* return_type);

  static FunctionDecl* Create(ASTContext& ctx, SourceRange loc,
                              std::string name, std::vector<VarDecl*> args,
                              Type* type, Type* return_type, Stmt* body);

  void AddLocal(Decl* decl) { locals_.push_back(decl); }

  void AddLocals(const std::vector<Decl*>& decls) {
    locals_.insert(locals_.end(), decls.begin(), decls.end());
  }

  std::vector<Decl*> GetLocals() { return locals_; }

  [[nodiscard]] bool IsMain() const { return GetName() == "main"; }

  Type* GetReturnType() { return return_type_; }

  Stmt* GetBody() { return body_; }

  void SetBody(Stmt* body) { body_ = body; }

  void SetParams(std::vector<VarDecl*> params) { args_ = std::move(params); }
  VarDecl* GetParam(std::size_t index) { return args_[index]; }

  [[nodiscard]] std::size_t GetParamNum() const { return args_.size(); }

  void dump(int indent) const override;

  void GenCode(CodeGen& gen) override;
};

// FIXME: Does RecordDecl has a type?
class RecordDecl : public Decl {
  std::vector<VarDecl*> members_;

  RecordDecl(SourceRange loc, std::string name, std::vector<VarDecl*> members)
      : Decl(std::move(loc), std::move(name), /*type=*/nullptr),
        members_(std::move(members)) {}

 public:
  static RecordDecl* Create(ASTContext& ctx, SourceRange loc, std::string name,
                            std::vector<VarDecl*> members);

  VarDecl* GetMember(std::size_t index) { return members_[index]; }

  [[nodiscard]] std::size_t GetMemberNum() const { return members_.size(); }

  void dump(int indent) const override;

  void GenCode(CodeGen& gen) override;
};
}  // namespace jcc
