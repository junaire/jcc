#include "jcc/type.h"

#include "jcc/ast_context.h"
#include "jcc/common.h"

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
      jcc_unimplemented();
    case Func:
      // TBD
      jcc_unimplemented();
    case Array:
      // TBD
      jcc_unimplemented();
    default:
      jcc_unimplemented();
  }
  return false;
}

Type* Type::CreateEnumType(ASTContext& ctx) {
  void* mem = ctx.Allocate<Type>();
  return new (mem) Type(TypeKind::Enum, 4, 4);
}

#define INIT_TYPE(KIND, SIZE, ALIGN) \
  void* mem = ctx.Allocate<Type>();  \
  return new (mem) Type(KIND, SIZE, ALIGN);

Type* Type::CreateVoidType(ASTContext& ctx) { INIT_TYPE(TypeKind::Void, 2, 1); }

Type* Type::CreateCharType(ASTContext& ctx, bool is_unsigned) {
  void* mem = ctx.Allocate<Type>();
  Type* type = new (mem) Type(TypeKind::Char, 1, 1);
  type->SetUnsigned(is_unsigned);
  return type;
}

Type* Type::CreateShortType(ASTContext& ctx, bool is_unsigned) {
  void* mem = ctx.Allocate<Type>();
  Type* type = new (mem) Type(TypeKind::Short, 2, 2);
  type->SetUnsigned(is_unsigned);
  return type;
}

Type* Type::CreateBoolType(ASTContext& ctx) {
  void* mem = ctx.Allocate<Type>();
  Type* type = new (mem) Type(TypeKind::Bool, 1, 1);
  return type;
}

Type* Type::CreateIntType(ASTContext& ctx, bool is_unsigned) {
  void* mem = ctx.Allocate<Type>();
  Type* type = new (mem) Type(TypeKind::Int, 4, 4);
  type->SetUnsigned(is_unsigned);
  return type;
}

Type* Type::CreateLongType(ASTContext& ctx, bool is_unsigned) {
  void* mem = ctx.Allocate<Type>();
  Type* type = new (mem) Type(TypeKind::Long, 8, 8);
  type->SetUnsigned(is_unsigned);
  return type;
}

Type* Type::CreateFloatType(ASTContext& ctx) {
  INIT_TYPE(TypeKind::Float, 4, 4);
}

Type* Type::CreateDoubleType(ASTContext& ctx, bool is_long) {
  int size_and_alignment = is_long ? 16 : 8;
  INIT_TYPE(TypeKind::Double, size_and_alignment, size_and_alignment);
}
#undef INIT_TYPE

Type* Type::CreatePointerType(ASTContext& ctx, Type* base) {
  void* mem = ctx.Allocate<PointerType>();
  auto* type = new (mem) PointerType(TypeKind::Ptr, 8, 8);
  type->SetBase(base);
  type->SetUnsigned();
  return type;
}

Type* Type::CreateFuncType(ASTContext& ctx, Type* return_type) {
  void* mem = ctx.Allocate<FunctionType>();
  auto* type = new (mem) FunctionType(TypeKind::Func, 1, 1);

  type->SetReturnType(return_type);
  return type;
}

Type* Type::CreateArrayType(ASTContext& ctx, Type* base, std::size_t len) {
  void* mem = ctx.Allocate<ArrayType>();
  auto* type = new (mem)
      ArrayType(TypeKind::Array, base->GetSize() * len, base->GetAlignment());
  type->SetBase(base);
  type->SetLength(len);
  return type;
}

Type* Type::CreateStructType(ASTContext& ctx) {
  void* mem = ctx.Allocate<StructType>();
  return new (mem) StructType(TypeKind::Struct, 0, 1);
}
