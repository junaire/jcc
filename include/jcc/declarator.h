#pragma once

#include "jcc/ast_context.h"
#include "jcc/type.h"

namespace jcc {

class DeclSpec {
  friend class Declarator;

 public:
  enum class StorageClassSpec {
    Typedef,
    Extern,
    Static,
    ThreadLocal,
    Auto,
    Register,
    Unspecified
  };

  enum class TypeQual { Const, Restrict, Volatile, Atomic, Unspecified };

  enum class FunctionSpec { Inline, NoReturn, Unspecified };

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

  [[nodiscard]] StorageClassSpec GetStorageClassSpec() const {
    return storage_class_spec_;
  }

  [[nodiscard]] TypeQual GetTypeQual() const { return type_qual_; }

  [[nodiscard]] TypeSpecKind GetTypeSpec() const { return type_spec_kind_; }

  [[nodiscard]] FunctionSpec GetFunctionSpec() const { return funtion_spec_; }

  [[nodiscard]] TypeSpecKind GetTypeSpecKind() const { return type_spec_kind_; }

  [[nodiscard]] TypeSpecWidth GetTypeSpecWidth() const {
    return type_spec_width_;
  }

  [[nodiscard]] TypeSpecSign GetTypeSpecSign() const { return type_spec_sign_; }

  void SetStorageClassSpec(StorageClassSpec spec) {
    storage_class_spec_ = spec;
  }

  void SetFunctionSpec(FunctionSpec spec) { funtion_spec_ = spec; }

  void SetTypeQual(TypeQual qual) { type_qual_ = qual; }

  void SetTypeSpecKind(TypeSpecKind kind) { type_spec_kind_ = kind; }

  void setTypeSpecWidth(TypeSpecWidth width) { type_spec_width_ = width; }

  void setTypeSpecSign(TypeSpecSign sign) { type_spec_sign_ = sign; }

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
        jcc_unreachable("Unknown type specifier when generating type!");
    }
  }

  Type* GetType() { return type_; }

  [[nodiscard]] bool IsTypedef() const {
    return storage_class_spec_ == StorageClassSpec::Typedef;
  }

  [[nodiscard]] bool IsExtern() const {
    return storage_class_spec_ == StorageClassSpec::Extern;
  }

  [[nodiscard]] bool isRegister() const {
    return storage_class_spec_ == StorageClassSpec::Register;
  }

  [[nodiscard]] bool IsStatic() const {
    return storage_class_spec_ == StorageClassSpec::Static;
  }

  [[nodiscard]] bool IsThreadLocal() const {
    return storage_class_spec_ == StorageClassSpec::ThreadLocal;
  }

  [[nodiscard]] bool IsAuto() const {
    return storage_class_spec_ == StorageClassSpec::Auto;
  }

  [[nodiscard]] bool IsInline() const {
    return funtion_spec_ == FunctionSpec::Inline;
  }

  [[nodiscard]] bool IsNoReturn() const {
    return funtion_spec_ == FunctionSpec::NoReturn;
  }

  [[nodiscard]] bool IsConst() const { return type_qual_ == TypeQual::Const; }

  [[nodiscard]] bool IsRestrict() const {
    return type_qual_ == TypeQual::Restrict;
  }

  [[nodiscard]] bool IsAtomic() const { return type_qual_ == TypeQual::Atomic; }

  [[nodiscard]] bool IsVolatile() const {
    return type_qual_ == TypeQual::Volatile;
  }

  StorageClassSpec storage_class_spec_ = StorageClassSpec::Unspecified;
  TypeQual type_qual_ = TypeQual::Unspecified;
  FunctionSpec funtion_spec_ = FunctionSpec::Unspecified;
  TypeSpecKind type_spec_kind_ = TSK_Int;  // Yeah, implicit int, so evil
  TypeSpecWidth type_spec_width_ = TypeSpecWidth::Unspecified;
  TypeSpecSign type_spec_sign_ = TypeSpecSign::Unspecified;
  Type* type_ = nullptr;
  ASTContext& ctx_;
};

class Declarator {
  // Maybe a mutbale referecnce is enough
  friend class Parser;
  DeclSpec& decl_spec_;

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

  std::string GetName() { return decl_spec_.GetType()->GetNameAsString(); }

  void SetName(const Token& name) { decl_spec_.GetType()->SetName(name); }
};
}  // namespace jcc
