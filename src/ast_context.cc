#include "jcc/ast_context.h"

#include "jcc/type.h"

namespace jcc {

ASTContext::ASTContext() { CreateBuiltinTypes(); }

void ASTContext::CreateBuiltinTypes() {
  void_type_ = Type::CreateVoidType(*this);
  bool_type_ = Type::CreateBoolType(*this);

  char_type_ = Type::CreateCharType(*this, /*is_unsigned=*/false);
  short_type_ = Type::CreateShortType(*this, /*is_unsigned=*/false);
  int_type_ = Type::CreateIntType(*this, /*is_unsigned=*/false);
  long_type_ = Type::CreateLongType(*this, /*is_unsigned=*/false);

  uchar_type_ = Type::CreateCharType(*this, /*is_unsigned=*/true);
  ushort_type_ = Type::CreateShortType(*this, /*is_unsigned=*/true);
  uint_type_ = Type::CreateIntType(*this, /*is_unsigned=*/true);
  ulong_type_ = Type::CreateLongType(*this, /*is_unsigned=*/true);

  float_type_ = Type::CreateFloatType(*this);
  double_type_ = Type::CreateDoubleType(*this, /*is_long=*/false);
  ldouble_type_ = Type::CreateDoubleType(*this, /*is_long=*/true);
}

void ASTContext::RegisterUserType(Type* type) {
  user_defined_types_.push_back(type);
}
}  // namespace jcc
