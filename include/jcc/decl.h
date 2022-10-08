#pragma once

#include <string>
#include <string_view>
#include <vector>

#include "jcc/ast_node.h"
#include "jcc/source_location.h"

class Type;
class Stmt;
class ASTContext;

class Decl : public ASTNode {
  SourceRange loc_;

 protected:
  explicit Decl(SourceRange loc) : loc_(std::move(loc)) {}

 public:
  virtual void dump(int indent) const override{};
  virtual ~Decl() override = default;
};

class VarDecl : public Decl {
  Stmt* init_{nullptr};
  Type* type_;
  std::string name_;

  VarDecl(SourceRange loc, Stmt* init, Type* type, std::string name)
      : Decl(std::move(loc)),
        init_(init),
        type_(type),
        name_(std::move(name)) {}

 public:
  static VarDecl* Create(ASTContext& ctx, SourceRange loc, Stmt* init,
                         Type* type, std::string name);

  [[nodiscard]] std::string_view GetName() const { return name_; }

  Type* GetType() { return type_; }

  Stmt* GetInit() { return init_; }

  void SetInit(Stmt* init) { init_ = init; }

  [[nodiscard]] bool IsDefinition() const { return init_ == nullptr; }

  void dump(int indent) const override;
};

class FunctionDecl : public Decl {
  std::string name_;
  std::vector<VarDecl*> args_;
  Type* return_type_;
  Stmt* body_ = nullptr;

  FunctionDecl(SourceRange loc, std::string name, std::vector<VarDecl*> args,
               Type* return_type, Stmt* body)
      : Decl(std::move(loc)),
        name_(std::move(name)),
        args_(std::move(args)),
        return_type_(return_type),
        body_(body) {}

 public:
  static FunctionDecl* Create(ASTContext& ctx, SourceRange loc,
                              std::string name, std::vector<VarDecl*> args,
                              Type* return_type, Stmt* body);

  [[nodiscard]] std::string_view GetName() const { return name_; }

  Type* GetReturnType() { return return_type_; }

  Stmt* GetBody() { return body_; }

  VarDecl* GetParam(std::size_t index) { return args_[index]; }

  [[nodiscard]] std::size_t GetParamNum() const { return args_.size(); }

  void dump(int indent) const override;
};

class LabelDecl : public Decl {};

class EnumDecl : public Decl {};

class TypedefDecl : public Decl {};

class RecordDecl : public Decl {
  std::string name_;
  std::vector<VarDecl*> members_;

  RecordDecl(SourceRange loc, std::string name, std::vector<VarDecl*> members)
      : Decl(std::move(loc)),
        name_(std::move(name)),
        members_(std::move(members)) {}

 public:
  static RecordDecl* Create(ASTContext& ctx, SourceRange loc, std::string name,
                            std::vector<VarDecl*> members);

  [[nodiscard]] std::string_view GetName() const { return name_; }

  VarDecl* GetMember(std::size_t index) { return members_[index]; }

  [[nodiscard]] std::size_t GetMemberNum() const { return members_.size(); }

  void dump(int indent) const override;
};
