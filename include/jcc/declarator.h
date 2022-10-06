#pragma once

#include "jcc/ast_context.h"
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

  explicit DeclSpec(ASTContext& ctx) : ctx_(ctx) {}

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

  void SetType(Type* type) { type_ = type; }

  void GenerateType() {
    // What if it's a user-defined type?
    switch (GetTypeSpecWidth()) {
      case TypeSpecWidth::Short: {
        bool is_unsigned = (GetTypeSpecSign() == TypeSpecSign::Unsigned);
        type_ = is_unsigned ? ctx_.GetUShortType() : ctx_.GetShortType();
        return;
      }
      case TypeSpecWidth::Long:
      case TypeSpecWidth::LongLong: {
        bool is_unsigned = GetTypeSpecSign() == TypeSpecSign::Unsigned;
        type_ = is_unsigned ? ctx_.GetULongType() : ctx_.GetLongType();
        return;
      }
      default:
        break;
    }

    switch (GetTypeSpecKind()) {
      case TSK_Void:
        type_ = ctx_.GetVoidType();
        return;
      case TSK_Bool:
        type_ = ctx_.GetBoolType();
        return;
      case TSK_Char: {
        bool is_unsigned = GetTypeSpecSign() == TypeSpecSign::Unsigned;
        type_ = is_unsigned ? ctx_.GetUCharType() : ctx_.GetCharType();
        return;
      }
      case TSK_Int: {
        bool is_unsigned = GetTypeSpecSign() == TypeSpecSign::Unsigned;
        type_ = is_unsigned ? ctx_.GetUIntType() : ctx_.GetIntType();
        break;
      }
      case TSK_Float:
        type_ = ctx_.GetFloatType();
        break;
      case TSK_Double: {
        bool is_long = GetTypeSpecWidth() == TypeSpecWidth::Long;
        type_ = is_long ? ctx_.GetULongType() : ctx_.GetLongType();
        break;
      }
      default:
        jcc_unreachable();
    }
  }

  Type* GetType() { return type_; }

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

  // FIXME: Naming conversion?
  StorageClassSpec SCS;
  TypeQual TQ;
  FunctionSpec FS;
  TypeSpecKind TSK = TSK_Int;  // Yeah, implicit int, so evil
  TypeSpecWidth TSW = TypeSpecWidth::Unspecified;
  TypeSpecSign TSS = TypeSpecSign::Unspecified;
  Type* type_ = nullptr;
  ASTContext& ctx_;
};

class Declarator {
  // Maybe a mutbale referecnce is enough
  friend class Parser;
  DeclSpec& decl_spec_;
  Token name_;

 public:
  explicit Declarator(DeclSpec& decl_spec) : decl_spec_(decl_spec) {}

  DeclSpec& GetDeclSpec() { return decl_spec_; }

  Type* GetBaseType() {
    return decl_spec_.GetType();
    ;
  }

  [[nodiscard]] TypeKind GetTypeKind() const {
    return decl_spec_.type_->GetKind();
  }

  void SetType(Type* type) { decl_spec_.SetType(type); }

  std::string GetName() { return name_.GetAsString(); }
};
