#pragma once

#include "jcc/type.h"

class DeclSpec {
  friend class Declarator;

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

  [[nodiscard]] StorageClassSpec GetStorageClassSpec() const { return SCS; }

  [[nodiscard]] TypeQual GetTypeQual() const { return TQ; }

  [[nodiscard]] TypeSpecKind GetTypeSpec() const { return TSK; }

  [[nodiscard]] FunctionSpec GetFunctionSpec() const { return FS; }

  [[nodiscard]] TypeSpecKind GetTypeSpecKind() const { return TSK; }

  [[nodiscard]] TypeSpecWidth GetTypeSpecWidth() const { return TSW; }

  [[nodiscard]] TypeSpecSign GetTypeSpecSign() const { return TSS; }

  void SetStorageClassSpec(StorageClassSpec spec) { SCS = spec; }

  void SetFunctionSpec(FunctionSpec spec) { FS = spec; }

  void SetTypeQual(TypeQual qual) { TQ = qual; }

  void SetTypeSpecKind(TypeSpecKind kind) { TSK = kind; }

  void setTypeSpecWidth(TypeSpecWidth width) { TSW = width; }

  void setTypeSpecSign(TypeSpecSign sign) { TSS = sign; }

  void SetType(std::unique_ptr<Type> type) { type_ = std::move(type); }

  void GenerateType() {
    // What if it's a user-defined type?
    switch (GetTypeSpecWidth()) {
      case TypeSpecWidth::Short: {
        bool isUnsigned = (GetTypeSpecSign() == TypeSpecSign::Unsigned);
        type_ = Type::createShortType(isUnsigned);
        return;
      }
      case TypeSpecWidth::Long:
      case TypeSpecWidth::LongLong: {
        bool isUnsigned = (GetTypeSpecSign() == TypeSpecSign::Unsigned);
        type_ = Type::createLongType(isUnsigned);
        return;
      }
      default:
        break;
    }

    switch (GetTypeSpecKind()) {
      case TSK_Void:
        type_ = Type::createVoidType();
        return;
      case TSK_Bool:
        type_ = Type::createBoolType();
        return;
      case TSK_Char: {
        bool isUnsigned = (GetTypeSpecSign() == TypeSpecSign::Unsigned);
        type_ = Type::createCharType(isUnsigned);
        return;
      }
      case TSK_Int: {
        bool isUnsigned = (GetTypeSpecSign() == TypeSpecSign::Unsigned);
        type_ = Type::createIntType(isUnsigned);
        break;
      }
      case TSK_Float:
        type_ = Type::createFloatType();
        break;
      case TSK_Double: {
        bool isLong = (GetTypeSpecWidth() == TypeSpecWidth::Long);
        type_ = Type::createDoubleType(isLong);
        break;
      }
      default:
        jcc_unreachable();
    }
  }

  std::unique_ptr<Type> GetType() { return std::move(type_); }

  [[nodiscard]] bool IsTypedef() const {
    return SCS == StorageClassSpec::Typedef;
  }

  [[nodiscard]] bool IsExtern() const {
    return SCS == StorageClassSpec::Extern;
  }

  [[nodiscard]] bool isRegister() const {
    return SCS == StorageClassSpec::Register;
  }

  [[nodiscard]] bool IsStatic() const {
    return SCS == StorageClassSpec::Static;
  }

  [[nodiscard]] bool IsThreadLocal() const {
    return SCS == StorageClassSpec::ThreadLocal;
  }

  [[nodiscard]] bool IsAuto() const { return SCS == StorageClassSpec::Auto; }

  [[nodiscard]] bool IsInline() const { return FS == FunctionSpec::Inline; }

  [[nodiscard]] bool IsNoReturn() const { return FS == FunctionSpec::NoReturn; }

  [[nodiscard]] bool IsConst() const { return TQ == TypeQual::Const; }

  [[nodiscard]] bool IsRestrict() const { return TQ == TypeQual::Restrict; }

  [[nodiscard]] bool IsAtomic() const { return TQ == TypeQual::Atomic; }

  [[nodiscard]] bool IsVolatile() const { return TQ == TypeQual::Volatile; }

 private:
  StorageClassSpec SCS;
  TypeQual TQ;
  FunctionSpec FS;
  TypeSpecKind TSK = TSK_Int;  // Yeah, implicit int, so evil
  TypeSpecWidth TSW = TypeSpecWidth::Unspecified;
  TypeSpecSign TSS = TypeSpecSign::Unspecified;
  std::unique_ptr<Type> type_;
};

class Declarator {
  // Maybe a mutbale referecnce is enough
  friend class Parser;
  DeclSpec& decl_spec_;
  Token name_;

 public:
  explicit Declarator(DeclSpec& decl_spec) : decl_spec_(decl_spec) {}

  DeclSpec& getDeclSpec() { return decl_spec_; }

  std::unique_ptr<Type> GetBaseType() {
    return decl_spec_.GetType();
    ;
  }

  [[nodiscard]] TypeKind GetTypeKind() const {
    return decl_spec_.type_->GetKind();
  }

  void setType(std::unique_ptr<Type> type) {
    decl_spec_.SetType(std::move(type));
  }

  std::string GetName() { return name_.GetAsString(); }
};
