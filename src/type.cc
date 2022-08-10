#include "jcc/type.h"

bool Type::isCompatible(const Type& lhs, const Type& rhs) {
  if (&lhs == &rhs) {
    return true;
  }

  if (lhs.getKind() == rhs.getKind()) {
    return true;
  }

  // TODO(Jun): Consider origin type?

  switch (lhs.getKind()) {
    using enum TypeKind;
    case Char:
    case Short:
    case Int:
    case Long:
      return lhs.isUnsigned() == rhs.isUnsigned();
    case Float:
    case Double:
    case Ldouble:
      return true;
    case Ptr:
      // TBD
      jcc_unreachable();
    case Func:
      // TBD
      jcc_unreachable();
    case Array:
      // TBD
      jcc_unreachable();
    default:
      jcc_unreachable();
  }
  return false;
}

std::unique_ptr<Type> Type::createPointerType(std::unique_ptr<Type> base) {
  std::unique_ptr<PointerType> type =
      std::make_unique<PointerType>(TypeKind::Ptr, 8, 8);
  type->setBase(std::move(base));
  type->setUnsigned();
  return type;
}

std::unique_ptr<Type> Type::createEnumType() {
  return std::make_unique<Type>(TypeKind::Enum, 4, 4);
}

std::unique_ptr<Type> Type::createStructType() {
  return std::make_unique<Type>(TypeKind::Struct, 0, 1);
}

#define INIT_TYPE(KIND, SIZE, ALIGN) std::make_unique<Type>(KIND, SIZE, ALIGN);

std::unique_ptr<Type> Type::createVoidType() {
  return INIT_TYPE(TypeKind::Void, 2, 1);
}

std::unique_ptr<Type> Type::createBoolType() {
  return INIT_TYPE(TypeKind::Bool, 1, 1);
}

std::unique_ptr<Type> Type::createCharType(bool isUnsigned) {
  std::unique_ptr<Type> type = INIT_TYPE(TypeKind::Char, 1, 1);
  type->setUnsigned(isUnsigned);
  return type;
}

std::unique_ptr<Type> Type::createShortType(bool isUnsigned) {
  std::unique_ptr<Type> type = INIT_TYPE(TypeKind::Short, 2, 2);
  type->setUnsigned(isUnsigned);
  return type;
}

std::unique_ptr<Type> Type::createIntType(bool isUnsigned) {
  std::unique_ptr<Type> type = INIT_TYPE(TypeKind::Int, 4, 4);
  type->setUnsigned(isUnsigned);
  return type;
}

std::unique_ptr<Type> Type::createLongType(bool isUnsigned) {
  std::unique_ptr<Type> type = INIT_TYPE(TypeKind::Long, 8, 8);
  type->setUnsigned(isUnsigned);
  return type;
}

std::unique_ptr<Type> Type::createFloatType() {
  return INIT_TYPE(TypeKind::Float, 4, 4);
}

std::unique_ptr<Type> Type::createDoubleType(bool isLong) {
  int sizeAndAlignment = isLong ? 16 : 8;
  return INIT_TYPE(TypeKind::Double, sizeAndAlignment, sizeAndAlignment);
}
#undef INIT_TYPE
