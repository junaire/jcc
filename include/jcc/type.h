#pragma once

#include <concepts>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include "jcc/token.h"

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

class Type {
  TypeKind kind_;
  Qualifiers quals_ = Qualifiers::Unspecified;

  int size_ = 0;
  int alignment_ = 0;
  bool unsigned_ = false;

  Type* origin = nullptr;

  Token name_;

 public:
  Type() = default;

  explicit Type(TypeKind kind, int size, int alignment)
      : kind_(kind), size_(size), alignment_(alignment) {}

  template <typename Ty>
  requires std::convertible_to<Ty, Type> Ty* AsType() {
    return static_cast<Ty*>(this);
  }

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

  static std::unique_ptr<Type> CreatePointerType(std::unique_ptr<Type> base);

  static std::unique_ptr<Type> createEnumType();

  static std::unique_ptr<Type> createStructType();

  static std::unique_ptr<Type> createVoidType();

  static std::unique_ptr<Type> createBoolType();

  static std::unique_ptr<Type> createCharType(bool is_unsigned);

  static std::unique_ptr<Type> createShortType(bool is_unsigned);

  static std::unique_ptr<Type> createIntType(bool is_unsigned);

  static std::unique_ptr<Type> createLongType(bool is_unsigned);

  static std::unique_ptr<Type> createFloatType();

  static std::unique_ptr<Type> createDoubleType(bool is_long);

  static std::unique_ptr<Type> CreateFuncType(
      std::unique_ptr<Type> return_type);

  static std::unique_ptr<Type> CreateArrayType(std::unique_ptr<Type> base,
                                               std::size_t len);

  static bool IsCompatible(const Type& lhs, const Type& rhs);
};

class ArrayType : public Type {
  std::size_t len_ = 0;
  std::unique_ptr<Type> base_;

 public:
  ArrayType() = default;
  ArrayType(TypeKind kind, int size, int alignment)
      : Type(kind, size, alignment) {}

  [[nodiscard]] std::size_t GetLength() const { return len_; }

  [[nodiscard]] Type* GetBase() const { return base_.get(); }

  void SetBase(std::unique_ptr<Type> base) { base_ = std::move(base); }

  void SetLength(std::size_t len) { len_ = len; }
};

class PointerType : public Type {
  std::unique_ptr<Type> base_;

 public:
  PointerType() = default;
  PointerType(TypeKind kind, int size, int alignment)
      : Type(kind, size, alignment) {}

  std::unique_ptr<Type> GetBase() { return std::move(base_); }

  void SetBase(std::unique_ptr<Type> base) { base_ = std::move(base); }
};

class StructType : public Type {
  class StructMember : public Type {
    std::unique_ptr<Type> type_;
  };

  std::vector<std::unique_ptr<StructMember>> members_;

 public:
  StructType() = default;

  StructType(TypeKind kind, int size, int alignment)
      : Type(kind, size, alignment) {}
};

class FunctionType : public Type {
  std::unique_ptr<Type> returnType_;
  std::vector<std::unique_ptr<Type>> paramTypes_;

 public:
  FunctionType() = default;
  FunctionType(TypeKind kind, int size, int alignment)
      : Type(kind, size, alignment) {}

  std::unique_ptr<Type> GetReturnType() { return std::move(returnType_); }

  void setReturnType(std::unique_ptr<Type> type) {
    returnType_ = std::move(type);
  }

  Type* GetParamType(std::size_t idx) {
    assert(idx < paramTypes_.size() && "No more params!");
    return paramTypes_[idx].get();
  }

  void SetParams(std::vector<std::unique_ptr<Type>> params) {
    paramTypes_ = std::move(params);
  }

  [[nodiscard]] std::size_t GetParamSize() const { return paramTypes_.size(); }
};
