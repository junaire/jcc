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

  void setType(std::unique_ptr<Type> type) { type_ = std::move(type); }

  void constructSelfType() {
    // What if it's a user-defined type?
    switch (getTypeSpecWidth()) {
      case TypeSpecWidth::Short: {
        bool isUnsigned = (getTypeSpecSign() == TypeSpecSign::Unsigned);
        type_ = Type::createShortType(isUnsigned);
        return;
      }
      case TypeSpecWidth::Long:
      case TypeSpecWidth::LongLong: {
        bool isUnsigned = (getTypeSpecSign() == TypeSpecSign::Unsigned);
        type_ = Type::createLongType(isUnsigned);
        return;
      }
      default:
        break;
    }

    switch (getTypeSpecKind()) {
      case TSK_Void:
        type_ = Type::createVoidType();
        return;
      case TSK_Bool:
        type_ = Type::createBoolType();
        return;
      case TSK_Char: {
        bool isUnsigned = (getTypeSpecSign() == TypeSpecSign::Unsigned);
        type_ = Type::createCharType(isUnsigned);
        return;
      }
      case TSK_Int: {
        bool isUnsigned = (getTypeSpecSign() == TypeSpecSign::Unsigned);
        type_ = Type::createIntType(isUnsigned);
        break;
      }
      case TSK_Float:
        type_ = Type::createFloatType();
        break;
      case TSK_Double: {
        bool isLong = (getTypeSpecWidth() == TypeSpecWidth::Long);
        type_ = Type::createDoubleType(isLong);
        break;
      }
      default:
        jcc_unreachable();
    }
  }

  std::unique_ptr<Type> getType() { return std::move(type_); }

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
  std::unique_ptr<Type> type_;
};

class Declarator {
  // Maybe a mutbale referecnce is enough
  friend class Parser;
  DeclSpec& declSpec_;
  Token name_;

 public:
  explicit Declarator(DeclSpec& declSpec)
      : declSpec_(declSpec) {}

  DeclSpec& getDeclSpec() { return declSpec_; }

  std::unique_ptr<Type> getBaseType() {
    return declSpec_.getType();
    ;
  }

  [[nodiscard]] TypeKind getTypeKind() const {
    return declSpec_.type_->getKind();
  }

  void setType(std::unique_ptr<Type> type) {
    declSpec_.setType(std::move(type));
  }

  std::string getName() { return name_.getAsString(); }
};
