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
  TypeSpecKind TSK = TSK_Int;  // Yeah, implicit int, so evil
  TypeSpecWidth TSW = TypeSpecWidth::Unspecified;
  TypeSpecSign TSS = TypeSpecSign::Unspecified;
};

class Declarator {
  // Maybe a mutbale referecnce is enough
  DeclSpec declSpec_;
  ASTContext& ctx_;

 public:
  explicit Declarator(DeclSpec declSpec, ASTContext& ctx)
      : declSpec_(declSpec), ctx_(ctx) {}

  DeclSpec getDeclSpec() { return declSpec_; }

  Type* getBaseType() {
    // FIXME: If the type is builtin-tpye, then it's just a reference, and we
    // don't control its lifetime. However, if it's a user-defined type, we need
    // to find a way to delete it...
    Type* type;
    switch (declSpec_.getTypeSpecKind()) {
      using enum TypeKind;
      case Void:
        type = ctx_.getVoidType();
        break;
      case Bool:
        type = ctx_.getBoolType();
        break;
      case Char:
        type = ctx_.getCharType();
        break;
      case Short:
        type = ctx_.getShortType();
        break;
      case Int:
        type = ctx_.getIntType();
        break;
      case Long:
        type = ctx_.getLongType();
        break;
      case Float:
        type = ctx_.getFloatType();
        break;
      case Double:
        type = ctx_.getDoubleType();
        break;
      case Ldouble:
        type = ctx_.getLdoubleType();
        break;
      case Enum:
      case Ptr:
      case Func:
      case Array:
      case Vla:  // variable-length array
      case Struct:
      case Union:
        jcc_unreachable();
    }
    return type;
  }
};
