#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include "jcc/ast_context.h"
#include "jcc/source_location.h"
#include "jcc/type.h"

class Stmt;
class ASTContext;

class Decl {
  SourceRange loc_;

 protected:
  explicit Decl(SourceRange loc) : loc_(std::move(loc)) {}

 public:
  static Decl* create(ASTContext& ctx, SourceRange loc);
  virtual ~Decl() = default;
};

class VarDecl : public Decl {
  Stmt* init_{nullptr};
  std::unique_ptr<Type> type_;
  std::string name_;

  VarDecl(SourceRange loc, Stmt* init, std::unique_ptr<Type> type,
          std::string name)
      : Decl(std::move(loc)),
        init_(init),
        type_(std::move(type)),
        name_(std::move(name)) {}

 public:
  static VarDecl* create(ASTContext& ctx, SourceRange loc, Stmt* init,
                         std::unique_ptr<Type> type, std::string name);

  [[nodiscard]] std::string_view getName() const { return name_; }

  Type* getType() { return type_.get(); }

  Stmt* getInit() { return init_; }

  [[nodiscard]] bool isDefinition() const { return init_ == nullptr; }
};

class FunctionDecl : public Decl {
  std::string name_;
  std::vector<VarDecl*> args_;
  std::unique_ptr<Type> returnTy_;
  Stmt* body_{nullptr};

  FunctionDecl(SourceRange loc, std::string name, std::vector<VarDecl*> args,
               std::unique_ptr<Type> returnTy, Stmt* body)
      : Decl(std::move(loc)),
        name_(std::move(name)),
        args_(std::move(args)),
        returnTy_(std::move(returnTy)),
        body_(body) {}

 public:
  static FunctionDecl* create(ASTContext& ctx, SourceRange loc,
                              std::string name, std::vector<VarDecl*> args,
                              std::unique_ptr<Type> returnTy, Stmt* body);

  [[nodiscard]] std::string_view getName() const { return name_; }

  Type* getType() { return returnTy_.get(); }

  Stmt* getBody() { return body_; }

  VarDecl* getParam(std::size_t index) { return args_[index]; }

  [[nodiscard]] std::size_t getParamNum() const { return args_.size(); }
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
  static RecordDecl* create(ASTContext& ctx, SourceRange loc, std::string name,
                            std::vector<VarDecl*> members);

  [[nodiscard]] std::string_view getName() const { return name_; }

  VarDecl* getMember(std::size_t index) { return members_[index]; }

  [[nodiscard]] std::size_t getMemberNum() const { return members_.size(); }
};

