#include "jcc/ast_context.h"

#include "jcc/type.h"

namespace jcc {

Type* ASTContext::GetVoidType() { return Type::CreateVoidType(*this); }
Type* ASTContext::GetBoolType() { return Type::CreateBoolType(*this); }
Type* ASTContext::GetCharType() {
  return Type::CreateCharType(*this, /*is_unsigned=*/false);
}
Type* ASTContext::GetShortType() {
  return Type::CreateShortType(*this, /*is_unsigned=*/false);
}
Type* ASTContext::GetIntType() {
  return Type::CreateIntType(*this, /*is_unsigned=*/false);
}
Type* ASTContext::GetLongType() {
  return Type::CreateLongType(*this, /*is_unsigned=*/false);
}
Type* ASTContext::GetUCharType() {
  return Type::CreateCharType(*this, /*is_unsigned=*/true);
}
Type* ASTContext::GetUShortType() {
  return Type::CreateShortType(*this, /*is_unsigned=*/true);
}
Type* ASTContext::GetUIntType() {
  return Type::CreateIntType(*this, /*is_unsigned=*/true);
}
Type* ASTContext::GetULongType() {
  return Type::CreateLongType(*this, /*is_unsigned=*/true);
}
Type* ASTContext::GetFloatType() { return Type::CreateFloatType(*this); }
Type* ASTContext::GetDoubleType() {
  return Type::CreateDoubleType(*this, /*is_long=*/false);
}
Type* ASTContext::GetLDoubleType() {
  return Type::CreateDoubleType(*this, /*is_long=*/true);
}

void ASTContext::RegisterUserType(Type* type) {
  user_defined_types_.push_back(type);
}
}  // namespace jcc
