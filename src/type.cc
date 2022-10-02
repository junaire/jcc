#include "jcc/type.h"

bool Type::IsCompatible(const Type& lhs, const Type& rhs) {
  if (&lhs == &rhs) {
    return true;
  }

  if (lhs.GetKind() == rhs.GetKind()) {
    return true;
  }

  // TODO(Jun): Consider origin type?

  switch (lhs.GetKind()) {
    using enum TypeKind;
    case Char:
    case Short:
    case Int:
    case Long:
      return lhs.IsUnsigned() == rhs.IsUnsigned();
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

std::unique_ptr<Type> Type::CreatePointerType(std::unique_ptr<Type> base) {
  std::unique_ptr<PointerType> type =
      std::make_unique<PointerType>(TypeKind::Ptr, 8, 8);
  type->SetBase(std::move(base));
  type->SetUnsigned();
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

std::unique_ptr<Type> Type::createCharType(bool is_unsigned) {
  std::unique_ptr<Type> type = INIT_TYPE(TypeKind::Char, 1, 1);
  type->SetUnsigned(is_unsigned);
  return type;
}

std::unique_ptr<Type> Type::createShortType(bool is_unsigned) {
  std::unique_ptr<Type> type = INIT_TYPE(TypeKind::Short, 2, 2);
  type->SetUnsigned(is_unsigned);
  return type;
}

std::unique_ptr<Type> Type::createIntType(bool is_unsigned) {
  std::unique_ptr<Type> type = INIT_TYPE(TypeKind::Int, 4, 4);
  type->SetUnsigned(is_unsigned);
  return type;
}

std::unique_ptr<Type> Type::createLongType(bool is_unsigned) {
  std::unique_ptr<Type> type = INIT_TYPE(TypeKind::Long, 8, 8);
  type->SetUnsigned(is_unsigned);
  return type;
}

std::unique_ptr<Type> Type::createFloatType() {
  return INIT_TYPE(TypeKind::Float, 4, 4);
}

std::unique_ptr<Type> Type::createDoubleType(bool is_long) {
  int sizeAndAlignment = is_long ? 16 : 8;
  return INIT_TYPE(TypeKind::Double, sizeAndAlignment, sizeAndAlignment);
}
#undef INIT_TYPE

std::unique_ptr<Type> Type::CreateFuncType(std::unique_ptr<Type> return_type) {
  std::unique_ptr<Type> type =
      std::make_unique<FunctionType>(TypeKind::Func, 1, 1);
  type->AsType<FunctionType>()->setReturnType(std::move(return_type));
  return type;
}

std::unique_ptr<Type> Type::CreateArrayType(std::unique_ptr<Type> base,
                                            std::size_t len) {
  std::unique_ptr<Type> type = std::make_unique<ArrayType>(
      TypeKind::Array, base->GetSize() * len, base->GetAlignment());
  type->AsType<ArrayType>()->SetBase(std::move(base));
  type->AsType<ArrayType>()->SetLength(len);
  return type;
}
