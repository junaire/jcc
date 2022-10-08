#pragma once

#include <concepts>
#include <string>
#include <string_view>
#include <vector>

#include "jcc/ast_node.h"
#include "jcc/common.h"
#include "jcc/token.h"

class ASTContext;

enum class Qualifiers {
  Unspecified,
  Const,
  Restrict,
  Volatile,
};

enum class TypeKind {
  Void,
  Bool,
  Char,
  Short,
  Int,
  Long,
  Float,
  Double,
  Ldouble,
  Enum,
  Ptr,
  Func,
  Array,
  Vla,  // variable-length array
  Struct,
  Union,
};

class Type : public ASTNode {
  TypeKind kind_;
  Qualifiers quals_ = Qualifiers::Unspecified;

  int size_ = 0;
  int alignment_ = 0;
  bool unsigned_ = false;

  Type* origin_ = nullptr;

  Token name_;

 public:
  Type() = default;

  explicit Type(TypeKind kind, int size, int alignment)
      : kind_(kind), size_(size), alignment_(alignment) {}

  template <typename Ty>
  requires std::convertible_to<Ty, Type> Ty* AsType() {
    return static_cast<Ty*>(this);
  }

  virtual void dump(int indent) const { jcc_unreachable(); }

  [[nodiscard]] bool HasQualifiers() const {
    return quals_ != Qualifiers::Unspecified;
  }

  [[nodiscard]] bool IsConst() const { return quals_ == Qualifiers::Const; }

  [[nodiscard]] bool IsRestrict() const {
    return quals_ == Qualifiers::Restrict;
  }

  [[nodiscard]] bool IsVolatile() const {
    return quals_ == Qualifiers::Volatile;
  }

  template <TypeKind ty, TypeKind... tyKinds>
  [[nodiscard]] bool Is() const {
    if constexpr (sizeof...(tyKinds) != 0) {
      return kind_ == ty || Is<tyKinds...>();
    }
    return false;
  }

  [[nodiscard]] TypeKind GetKind() const { return kind_; }

  [[nodiscard]] bool IsInteger() const {
    using enum TypeKind;
    return this->Is<Bool, Char, Short, Int>();
  }

  [[nodiscard]] bool IsFloating() const {
    using enum TypeKind;
    return this->Is<Double, Ldouble, Float>();
  }

  [[nodiscard]] bool IsNumeric() const { return IsInteger() || IsFloating(); }

  [[nodiscard]] bool IsPointer() const { return this->Is<TypeKind::Ptr>(); }

  void SetName(Token name) { name_ = name; }

  void SetSizeAlign(int size, int alignment) {
    size_ = size;
    alignment_ = alignment;
  }

  void SetUnsigned(bool usg = true) { unsigned_ = usg; }

  [[nodiscard]] std::string GetName() const { return name_.GetAsString(); }

  [[nodiscard]] int GetSize() const { return size_; }

  [[nodiscard]] int GetAlignment() const { return alignment_; }

  [[nodiscard]] bool IsUnsigned() const { return unsigned_; }

  static Type* CreatePointerType(ASTContext& ctx, Type* base);

  static Type* CreateEnumType(ASTContext& ctx);

  static Type* CreateStructType(ASTContext& ctx);

  static Type* CreateVoidType(ASTContext& ctx);

  static Type* CreateBoolType(ASTContext& ctx);

  static Type* CreateCharType(ASTContext& ctx, bool is_unsigned);

  static Type* CreateShortType(ASTContext& ctx, bool is_unsigned);

  static Type* CreateIntType(ASTContext& ctx, bool is_unsigned);

  static Type* CreateLongType(ASTContext& ctx, bool is_unsigned);

  static Type* CreateFloatType(ASTContext& ctx);

  static Type* CreateDoubleType(ASTContext& ctx, bool is_long);

  static Type* CreateFuncType(ASTContext& ctx, Type* return_type);

  static Type* CreateArrayType(ASTContext& ctx, Type* base, std::size_t len);

  static bool IsCompatible(const Type& lhs, const Type& rhs);
};

class ArrayType : public Type {
  std::size_t len_ = 0;
  Type* base_;

 public:
  ArrayType() = default;
  ArrayType(TypeKind kind, int size, int alignment)
      : Type(kind, size, alignment) {}

  [[nodiscard]] std::size_t GetLength() const { return len_; }

  [[nodiscard]] Type* GetBase() const { return base_; }

  void SetBase(Type* base) { base_ = base; }

  void SetLength(std::size_t len) { len_ = len; }
};

class PointerType : public Type {
  Type* base_;

 public:
  PointerType() = default;
  PointerType(TypeKind kind, int size, int alignment)
      : Type(kind, size, alignment) {}

  Type* GetBase() { return base_; }

  void SetBase(Type* base) { base_ = base; }
};

class StructType : public Type {
  class StructMember : public Type {
    Type* type_;
  };

  std::vector<StructMember*> members_;

 public:
  StructType() = default;

  StructType(TypeKind kind, int size, int alignment)
      : Type(kind, size, alignment) {}
};

class FunctionType : public Type {
  Type* return_type_;
  std::vector<Type*> param_types_;

 public:
  FunctionType() = default;
  FunctionType(TypeKind kind, int size, int alignment)
      : Type(kind, size, alignment) {}

  Type* GetReturnType() { return return_type_; }

  void SetReturnType(Type* type) { return_type_ = type; }

  Type* GetParamType(std::size_t idx) {
    assert(idx < param_types_.size() && "No more params!");
    return param_types_[idx];
  }

  void SetParams(std::vector<Type*> params) {
    param_types_ = std::move(params);
  }

  [[nodiscard]] std::size_t GetParamSize() const { return param_types_.size(); }
};
