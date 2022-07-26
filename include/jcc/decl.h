#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <vector>

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
  Type type_;
  std::string name_;

  VarDecl(SourceRange loc, Stmt* init, Type type, std::string name)
      : Decl(std::move(loc)),
        init_(init),
        type_(std::move(type)),
        name_(std::move(name)) {}

 public:
  static VarDecl* create(ASTContext& ctx, SourceRange loc, Stmt* init,
                         Type type, std::string name);

  [[nodiscard]] std::string_view getName() const { return name_; }

  Type getType() { return type_; }

  Stmt* getInit() { return init_; }

  [[nodiscard]] bool isDefinition() const { return init_ == nullptr; }
};

class FunctionDecl : public Decl {
  std::string name_;
  std::vector<VarDecl*> args_;
  Type returnTy_;
  Stmt* body_{nullptr};

  FunctionDecl(SourceRange loc, std::string name, std::vector<VarDecl*> args,
               Type returnTy, Stmt* body)
      : Decl(std::move(loc)),
        name_(std::move(name)),
        args_(std::move(args)),
        returnTy_(std::move(returnTy)),
        body_(body) {}

 public:
  static FunctionDecl* create(ASTContext& ctx, SourceRange loc,
                              std::string name, std::vector<VarDecl*> args,
                              Type returnTy, Stmt* body);

  [[nodiscard]] std::string_view getName() const { return name_; }

  Type getType() { return returnTy_; }

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

class DeclSpec {
 public:
  enum class StorageClassSpec {
    Typedef,
    Extern,
    Static,
    ThreadLocal,
    Auto,
    Register
  };

  enum class TypeQual { Const, Restrict, Volatile, Atomic };

  enum class FunctionSpec { Inline, NoReturn };

  using TypeSpecKind = TypeKind;

  static const TypeSpecKind TSK_Void = TypeKind::Void;
  static const TypeSpecKind TSK_Bool = TypeKind::Bool;
  static const TypeSpecKind TSK_Char = TypeKind::Char;
  static const TypeSpecKind TSK_Int = TypeKind::Int;
  static const TypeSpecKind TSK_Float = TypeKind::Float;
  static const TypeSpecKind TSK_Double = TypeKind::Double;
  static const TypeSpecKind TSK_Enum = TypeKind::Enum;
  static const TypeSpecKind TSK_Union = TypeKind::Union;
  static const TypeSpecKind TSK_Struct = TypeKind::Struct;

  enum class TypeSpecWidth { Unspecified, Short, Long, LongLong };

  enum class TypeSpecSign { Unspecified, Signed, Unsigned };

  DeclSpec() = default;

  [[nodiscard]] StorageClassSpec getStorageClassSpec() const { return SCS; }

  [[nodiscard]] TypeQual getTypeQual() const { return TQ; }

  [[nodiscard]] TypeSpecKind getTypeSpec() const { return TSK; }

  [[nodiscard]] FunctionSpec getFunctionSpec() const { return FS; }

  [[nodiscard]] TypeSpecKind getTypeSpecKind() const { return TSK; }

  [[nodiscard]] TypeSpecWidth getTypeSpecWidth() const { return TSW; }

  [[nodiscard]] TypeSpecSign getTypeSpecSign() const { return TSS; }

  void setStorageClassSpec(StorageClassSpec spec) { SCS = spec; }

  void setFunctionSpec(FunctionSpec spec) { FS = spec; }

  void setTypeQual(TypeQual qual) { TQ = qual; }

  void setTypeSpecKind(TypeSpecKind kind) { TSK = kind; }

  void setTypeSpecWidth(TypeSpecWidth width) { TSW = width; }

  void setTypeSpecSign(TypeSpecSign sign) { TSS = sign; }

  [[nodiscard]] bool isTypedef() const {
    return SCS == StorageClassSpec::Typedef;
  }

  [[nodiscard]] bool isExtern() const {
    return SCS == StorageClassSpec::Extern;
  }

  [[nodiscard]] bool isRegister() const {
    return SCS == StorageClassSpec::Register;
  }

  [[nodiscard]] bool isStatic() const {
    return SCS == StorageClassSpec::Static;
  }

  [[nodiscard]] bool isThreadLocal() const {
    return SCS == StorageClassSpec::ThreadLocal;
  }

  [[nodiscard]] bool isAuto() const { return SCS == StorageClassSpec::Auto; }

  [[nodiscard]] bool isInline() const { return FS == FunctionSpec::Inline; }

  [[nodiscard]] bool isNoReturn() const { return FS == FunctionSpec::NoReturn; }

  [[nodiscard]] bool isConst() const { return TQ == TypeQual::Const; }

  [[nodiscard]] bool isRestrict() const { return TQ == TypeQual::Restrict; }

  [[nodiscard]] bool isAtomic() const { return TQ == TypeQual::Atomic; }

  [[nodiscard]] bool isVolatile() const { return TQ == TypeQual::Volatile; }

 private:
  StorageClassSpec SCS;
  TypeQual TQ;
  FunctionSpec FS;
  TypeSpecKind TSK;
  TypeSpecWidth TSW = TypeSpecWidth::Unspecified;
  TypeSpecSign TSS = TypeSpecSign::Unspecified;
};

class Declarator {
  DeclSpec declSpec_;

 public:
  explicit Declarator(DeclSpec declSpec) : declSpec_(declSpec) {}

  DeclSpec getDeclSpec() { return declSpec_; }
};
