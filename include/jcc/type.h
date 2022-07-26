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
  Token name_;
  Qualifiers quals_ = Qualifiers::Unspecified;

  std::unique_ptr<Type> base_;

  int size_ = 0;
  int alignment_ = 0;

  bool unsigned_ = false;

 public:
  Type() = default;

  explicit Type(TypeKind kind, int size, int alignment)
      : kind_(kind), size_(size), alignment_(alignment) {}

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

  void setBase(std::unique_ptr<Type> base) { base_ = std::move(base); }

  void setUnsigned(bool usg = true) { unsigned_ = usg; }

  [[nodiscard]] std::string_view getName() const { return name_.getName(); }

  [[nodiscard]] int getSize() const { return size_; }

  [[nodiscard]] int getAlignment() const { return alignment_; }

  [[nodiscard]] Type* getBase() const { return base_.get(); }

  [[nodiscard]] bool isUnsigned() const { return unsigned_; }

  static Type createPointerToType(std::unique_ptr<Type> base) {
    Type type(TypeKind::Ptr, 8, 8);
    type.setBase(std::move(base));
    type.setUnsigned();
    return type;
  }

  static Type createEnumType() { return Type(TypeKind::Enum, 4, 4); }

  static Type createStructType() { return Type(TypeKind::Struct, 0, 1); }
};
