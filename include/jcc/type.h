#pragma once

#include <concepts>
#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

enum class Qualifiers : std::uint8_t {
  Const = 1 << 0,
  Restrict = 1 << 1,
  Volatile = 1 << 2,
};

// FIXME: Do we really need bitmask here?
enum class Kind : std::uint32_t {
  Void = 1 << 0,
  Bool = 1 << 1,
  Char = 1 << 2,
  Short = 1 << 3,
  Int = 1 << 4,
  Long = 1 << 5,
  Float = 1 << 6,
  Double = 1 << 7,
  Ldouble = 1 << 8,
  Enum = 1 << 9,
  Ptr = 1 << 10,
  Func = 1 << 11,
  Array = 1 << 12,
  Vla = 1 << 13,  // variable-length array
  Struct = 1 << 14,
  Union = 1 << 15,
};

class Type {
  std::string name_;
  std::uint32_t kind_;

  int size_;
  int alignment_;

  std::uint8_t quals_;

 public:
  Type() = default;

  explicit Type(Kind kind) : kind_(static_cast<std::uint32_t>(kind)) {}

  Type(Kind kind, std::string name)
      : kind_(static_cast<std::uint32_t>(kind)), name_(std::move(name)) {}

  template <typename Ty>
  requires std::convertible_to<Ty, Type> Ty* asType() {
    return static_cast<Ty*>(this);
  }

  [[nodiscard]] int getSize() const { return size_; }

  [[nodiscard]] int getAlignment() const { return alignment_; }

  template <Qualifiers qualifier, Qualifiers... otherQuals>
  void setQualifiers() {
    quals_ |= static_cast<std::uint8_t>(qualifier);
    if constexpr (sizeof...(otherQuals) > 0) {
      setQualifiers<otherQuals...>();
    }
  }

  [[nodiscard]] bool isConst() const {
    return (quals_ & static_cast<std::uint8_t>(Qualifiers::Const)) != 0;
  }

  [[nodiscard]] bool isRestrict() const {
    return (quals_ & static_cast<std::uint8_t>(Qualifiers::Restrict)) != 0;
  }

  [[nodiscard]] bool isVolatile() const {
    return (quals_ & static_cast<std::uint8_t>(Qualifiers::Volatile)) != 0;
  }

  [[nodiscard]] bool hasQualifiers() const { return quals_ != 0; }

  template <Kind kind, Kind... otherKinds>
  void setKinds() {
    kind_ |= static_cast<std::uint8_t>(kind);
    if constexpr (sizeof...(otherKinds) > 0) {
      setKinds<otherKinds...>();
    }
  }

  template <Kind ty, Kind... tyKinds>
  [[nodiscard]] bool is() const {
    if constexpr (sizeof...(tyKinds) != 0) {
      return (kind_ & static_cast<std::uint32_t>(ty)) || is<tyKinds...>();
    }
    return false;
  }

  [[nodiscard]] bool isInteger() const {
    return this->is<Kind::Bool, Kind::Char, Kind::Short, Kind::Int>();
  }

  [[nodiscard]] bool isFloating() const {
    return this->is<Kind::Double, Kind::Ldouble, Kind::Float>();
  }

  [[nodiscard]] bool isNumeric() const { return isInteger() || isFloating(); }

  [[nodiscard]] bool isPointer() const { return this->is<Kind::Ptr>(); }

  [[nodiscard]] std::string_view getName() const { return name_; }

  void setName(std::string name) { name_ = std::move(name); }

  void setSizeAlign(int size, int alignment) {
    size_ = size;
    alignment_ = alignment;
  }
};
