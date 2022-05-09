#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include "jcc/source_location.h"

class Stmt;
class Type;

class Decl {
  SourceLocation loc_;

 public:
  explicit Decl(SourceLocation loc) : loc_(std::move(loc)) {}
  virtual ~Decl() = default;
};

class VarDecl : public Decl {
  Stmt* init_{nullptr};
  std::unique_ptr<Type> type_{nullptr};
  std::string name_;

 public:
  [[nodiscard]] std::string_view getName() const { return name_; }
  Type* getType() { return type_.get(); }
  Stmt* getInit() { return init_; }
  [[nodiscard]] bool isDefinition() const { return init_ == nullptr; }
};

class FunctionDecl : public Decl {
  std::unique_ptr<Type> returnTy_{nullptr};
  std::string name_;
  std::vector<VarDecl*> args_;
  Stmt* body_{nullptr};

 public:
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

 public:
  [[nodiscard]] std::string_view getName() const { return name_; }
  VarDecl* getMember(std::size_t index) { return members_[index]; }
  [[nodiscard]] std::size_t getMemberNum() const { return members_.size(); }
};
