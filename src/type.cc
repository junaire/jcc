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

std::unique_ptr<Type> Type::createPointerType(Type* base) {
  auto type = std::make_unique<Type>(TypeKind::Ptr, 8, 8);
  type->setBase(base);
  type->setUnsigned();
  return type;
}

std::unique_ptr<Type> Type::createEnumType() {
  return std::make_unique<Type>(TypeKind::Enum, 4, 4);
}

std::unique_ptr<Type> Type::createStructType() {
  return std::make_unique<Type>(TypeKind::Struct, 0, 1);
}
