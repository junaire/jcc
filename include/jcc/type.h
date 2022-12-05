#pragma once

#include <concepts>
#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

#include "fmt/format.h"
#include "jcc/common.h"
#include "jcc/token.h"

namespace jcc {

class ASTContext;

enum class Qualifiers : uint8_t {
  Unspecified = 0,
  Const = 1,
  Restrict = 2,
  Volatile = 3,
  Atomic = 4
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
  Union
};

class Type {
  TypeKind kind_;
  Qualifiers quals_ = Qualifiers::Unspecified;

  Token name_;

  size_t size_ = 0;
  size_t alignment_ = 0;
  bool unsigned_ = false;

  Type* origin_ = nullptr;

 public:
  Type() = default;
  virtual ~Type();

  explicit Type(TypeKind kind, size_t size, size_t alignment)
      : kind_(kind), size_(size), alignment_(alignment) {}

  template <typename Ty>
  requires std::convertible_to<Ty, Type> Ty* AsType() {
    return static_cast<Ty*>(this);
  }

  virtual void dump(int) const { fmt::print("{}\n", GetNameAsString()); }

  [[nodiscard]] bool HasQualifiers() const {
    return quals_ != Qualifiers::Unspecified;
  }
  void SetQualifiers(Qualifiers quals) { quals_ = quals; }

  [[nodiscard]] bool IsConst() const { return quals_ == Qualifiers::Const; }

  [[nodiscard]] bool IsRestrict() const {
    return quals_ == Qualifiers::Restrict;
  }

  [[nodiscard]] bool IsVolatile() const {
    return quals_ == Qualifiers::Volatile;
  }

  template <TypeKind ty>
  [[nodiscard]] bool Is() const {
    return kind_ == ty;
  }

  template <TypeKind kind, TypeKind... kinds>
  [[nodiscard]] bool IsOneOf() const {
    if (Is<kind>()) {
      return true;
    }
    if constexpr (sizeof...(kinds) > 0) {
      return IsOneOf<kinds...>();
    }
    return false;
  }

  [[nodiscard]] TypeKind GetKind() const { return kind_; }

  [[nodiscard]] bool IsInteger() const {
    using enum TypeKind;
    return this->IsOneOf<Bool, Char, Short, Int>();
  }

  [[nodiscard]] bool IsFloating() const {
    using enum TypeKind;
    return this->IsOneOf<Double, Ldouble, Float>();
  }

  [[nodiscard]] bool IsNumeric() const { return IsInteger() || IsFloating(); }

  [[nodiscard]] bool IsPointer() const { return this->Is<TypeKind::Ptr>(); }

  void SetName(const Token& name) { name_ = name; }

  Token GetName() { return name_; }

  void SetSizeAlign(size_t size, size_t alignment) {
    size_ = size;
    alignment_ = alignment;
  }

  void SetUnsigned(bool usg = true) { unsigned_ = usg; }

  [[nodiscard]] std::string GetNameAsString() const {
    assert(name_.IsValid() && "Can't access name of non-user-defined types!");
    return name_.GetAsString();
  }

  [[nodiscard]] std::size_t GetSize() const { return size_; }

  [[nodiscard]] size_t GetAlignment() const { return alignment_; }

  [[nodiscard]] bool IsUnsigned() const { return unsigned_; }

  static Type* CreatePointerType(ASTContext& ctx, Type* base);

  static Type* CreateEnumType(ASTContext& ctx);

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

  // struct or union.
  static Type* CreateRecordType(ASTContext& ctx, TypeKind kind);

  static bool IsCompatible(const Type& lhs, const Type& rhs);
};

class ArrayType : public Type {
  std::size_t len_ = 0;
  Type* base_;

 public:
  ArrayType() = default;
  ~ArrayType() override;

  ArrayType(TypeKind kind, std::size_t size, size_t alignment)
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
  ~PointerType() override;

  PointerType(TypeKind kind, size_t size, size_t alignment)
      : Type(kind, size, alignment) {}

  Type* GetBase() { return base_; }

  void SetBase(Type* base) { base_ = base; }
};

class RecordType : public Type {
  std::vector<Type*> members_;

 public:
  RecordType(TypeKind kind, size_t size, size_t alignment)
      : Type(kind, size, alignment) {}
  ~RecordType() override;

  void SetMembers(const std::vector<Type*>& members) { members_ = members; }

  [[nodiscard]] std::size_t GetMemberSize() const { return members_.size(); }

  Type* GetMember(std::size_t idx) { return members_[idx]; }
};

class FunctionType : public Type {
  Type* return_type_;
  std::vector<Type*> param_types_;

 public:
  FunctionType() = default;
  ~FunctionType() override;

  FunctionType(TypeKind kind, size_t size, size_t alignment)
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
}  // namespace jcc
