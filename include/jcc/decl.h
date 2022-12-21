#pragma once

#include <string>
#include <string_view>
#include <vector>

#include "jcc/ast_node.h"

namespace jcc {
class Type;
class Stmt;
class ASTContext;

class Decl : public ASTNode {
  std::string name_;

 protected:
  explicit Decl(SourceRange loc, std::string name)
      : ASTNode(std::move(loc)), name_(std::move(name)) {}

 public:
  [[nodiscard]] std::string GetName() const { return name_; }

  ~Decl() override;
};

class VarDecl : public Decl {
  Stmt* init_ = nullptr;
  Type* type_ = nullptr;

  VarDecl(SourceRange loc, Stmt* init, Type* type, std::string name)
      : Decl(std::move(loc), std::move(name)), init_(init), type_(type) {}

 public:
  static VarDecl* Create(ASTContext& ctx, SourceRange loc, Stmt* init,
                         Type* type, std::string name);

  Type* GetType() { return type_; }

  Stmt* GetInit() { return init_; }

  void SetInit(Stmt* init) { init_ = init; }

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

  FunctionDecl(SourceRange loc, std::string name, Type* return_type)
      : Decl(std::move(loc), std::move(name)), return_type_(return_type) {}

  FunctionDecl(SourceRange loc, std::string name, std::vector<VarDecl*> args,
               Type* return_type, Stmt* body)
      : Decl(std::move(loc), std::move(name)),
        args_(std::move(args)),
        return_type_(return_type),
        body_(body) {}

 public:
  static FunctionDecl* Create(ASTContext& ctx, SourceRange loc,
                              const std::string& name, Type* return_type);

  static FunctionDecl* Create(ASTContext& ctx, SourceRange loc,
                              std::string name, std::vector<VarDecl*> args,
                              Type* return_type, Stmt* body);

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

class RecordDecl : public Decl {
  std::vector<VarDecl*> members_;

  RecordDecl(SourceRange loc, std::string name, std::vector<VarDecl*> members)
      : Decl(std::move(loc), std::move(name)), members_(std::move(members)) {}

 public:
  static RecordDecl* Create(ASTContext& ctx, SourceRange loc, std::string name,
                            std::vector<VarDecl*> members);

  VarDecl* GetMember(std::size_t index) { return members_[index]; }

  [[nodiscard]] std::size_t GetMemberNum() const { return members_.size(); }

  void dump(int indent) const override;

  void GenCode(CodeGen& gen) override;
};
}  // namespace jcc
