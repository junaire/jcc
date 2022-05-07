#pragma once

enum class Qualifiers { Const, Restrict, Volatile, None };

enum class Kind {
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
  Qualifiers quals_;
  Kind kind_;

  int size_;
  int alignment_;

 public:
  Type() = default;

  Type(Qualifiers quals, Kind kind) : quals_(quals), kind_(kind) {
    switch (kind_) {
      case Kind::Void:
        setSizeAlign(1, 1);
        break;
      case Kind::Bool:
        setSizeAlign(1, 1);
        break;
      case Kind::Char:
        setSizeAlign(1, 1);
        break;

      case Kind::Short:
        setSizeAlign(2, 2);
        break;

      case Kind::Int:
        setSizeAlign(4, 4);
        break;

      case Kind::Long:
        setSizeAlign(8, 8);
        break;

      case Kind::Float:
        setSizeAlign(4, 4);
        break;

      case Kind::Double:
        setSizeAlign(8, 8);
        break;

      case Kind::Ldouble:
        setSizeAlign(16, 16);
        break;

      case Kind::Enum:
        setSizeAlign(4, 4);
        break;

      case Kind::Ptr:
        setSizeAlign(4, 4);
        break;

      case Kind::Func:
        setSizeAlign(4, 4);
        break;

      case Kind::Array:
        setSizeAlign(4, 4);
        break;

      case Kind::Vla:
        setSizeAlign(8, 8);
        break;

      case Kind::Struct:
        setSizeAlign(0, 1);
        break;

      case Kind::Union:
        setSizeAlign(0, 1);
        break;
    }
  }

  [[nodiscard]] Kind getKind() const { return kind_; }

  [[nodiscard]] int getSize() const { return size_; }

  [[nodiscard]] int getAlignment() const { return alignment_; }

  [[nodiscard]] bool isConst() const { return quals_ == Qualifiers::Const; }

  [[nodiscard]] bool isRestrict() const {
    return quals_ == Qualifiers::Restrict;
  }

  [[nodiscard]] bool isVolatile() const {
    return quals_ == Qualifiers::Volatile;
  }

  template <Kind ty, Kind... tyKinds>
  [[nodiscard]] bool is() const {
    if constexpr (sizeof...(tyKinds) != 0) {
      return kind_ == ty || is<tyKinds...>();
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

 private:
  void setSizeAlign(int size, int alignment) {
    size_ = size;
    alignment_ = alignment;
  }
};
