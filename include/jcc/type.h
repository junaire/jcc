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
  requires std::convertible_to<Ty, Type> Ty* asType() {
    return static_cast<Ty*>(this);
  }

  [[nodiscard]] bool hasQualifiers() const {
    return quals_ != Qualifiers::Unspecified;
  }

  [[nodiscard]] bool isConst() const { return quals_ == Qualifiers::Const; }

  [[nodiscard]] bool isRestrict() const {
    return quals_ == Qualifiers::Restrict;
  }

  [[nodiscard]] bool isVolatile() const {
    return quals_ == Qualifiers::Volatile;
  }

  template <TypeKind ty, TypeKind... tyKinds>
  [[nodiscard]] bool is() const {
    if constexpr (sizeof...(tyKinds) != 0) {
      return kind_ == ty || is<tyKinds...>();
    }
    return false;
  }

  [[nodiscard]] TypeKind getKind() const { return kind_; }

  [[nodiscard]] bool isInteger() const {
    using enum TypeKind;
    return this->is<Bool, Char, Short, Int>();
  }

  [[nodiscard]] bool isFloating() const {
    using enum TypeKind;
    return this->is<Double, Ldouble, Float>();
  }

  [[nodiscard]] bool isNumeric() const { return isInteger() || isFloating(); }

  [[nodiscard]] bool isPointer() const { return this->is<TypeKind::Ptr>(); }

  void setName(Token name) { name_ = name; }

  void setSizeAlign(int size, int alignment) {
    size_ = size;
    alignment_ = alignment;
  }

  void setUnsigned(bool usg = true) { unsigned_ = usg; }

  [[nodiscard]] std::string_view getName() const { return name_.getName(); }

  [[nodiscard]] int getSize() const { return size_; }

  [[nodiscard]] int getAlignment() const { return alignment_; }

  [[nodiscard]] bool isUnsigned() const { return unsigned_; }

  static std::unique_ptr<Type> createPointerType(std::unique_ptr<Type> base);

  static std::unique_ptr<Type> createEnumType();

  static std::unique_ptr<Type> createStructType();

  static std::unique_ptr<Type> createVoidType();

  static std::unique_ptr<Type> createBoolType();

  static std::unique_ptr<Type> createCharType(bool isUnsigned);

  static std::unique_ptr<Type> createShortType(bool isUnsigned);

  static std::unique_ptr<Type> createIntType(bool isUnsigned);

  static std::unique_ptr<Type> createLongType(bool isUnsigned);

  static std::unique_ptr<Type> createFloatType();

  static std::unique_ptr<Type> createDoubleType(bool isLong);

  static std::unique_ptr<Type> createFuncType(std::unique_ptr<Type> returnType);

  static std::unique_ptr<Type> createArrayType(std::unique_ptr<Type> base,
                                               std::size_t len);

  static bool isCompatible(const Type& lhs, const Type& rhs);
};

class ArrayType : public Type {
  unsigned len_ = 0;
  std::unique_ptr<Type> base_;

 public:
  ArrayType() = default;
  ArrayType(TypeKind kind, int size, int alignment)
      : Type(kind, size, alignment) {}

  [[nodiscard]] unsigned getLength() const { return len_; }

  [[nodiscard]] Type* getBase() const { return base_.get(); }

  void setBase(std::unique_ptr<Type> base) { base_ = std::move(base); }

  void setLength(std::size_t len) { len_ = len; }
};

class PointerType : public Type {
  std::unique_ptr<Type> base_;

 public:
  PointerType() = default;
  PointerType(TypeKind kind, int size, int alignment)
      : Type(kind, size, alignment) {}

  std::unique_ptr<Type> getBase() { return std::move(base_); }

  void setBase(std::unique_ptr<Type> base) { base_ = std::move(base); }
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

  Type* getReturnType() { return returnType_.get(); }

  void setReturnType(std::unique_ptr<Type> type) {
    returnType_ = std::move(type);
  }

  Type* getParamType(unsigned idx) {
    assert(idx < paramTypes_.size() && "No more params!");
    return paramTypes_[idx].get();
  }

  void setParams(std::vector<std::unique_ptr<Type>> params) {
    paramTypes_ = std::move(params);
  }
};
