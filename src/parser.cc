#include "jcc/parser.h"

#include "jcc/common.h"
#include "jcc/decl.h"
#include "jcc/expr.h"
#include "jcc/lexer.h"
#include "jcc/token.h"
#include "jcc/type.h"

Parser::Parser(Lexer& lexer) : lexer_(lexer) { token_ = lexer_.Lex(); }

Token Parser::CurrentToken() { return token_; }

void Parser::ConsumeToken() {
  if (cache_) {
    token_ = *cache_;
    cache_ = std::nullopt;
  } else {
    token_ = lexer_.Lex();
  }
}

void Parser::MustConsumeToken(TokenKind expected) {
  if (CurrentToken().GetKind() == expected) {
    ConsumeToken();
    return;
  }
  jcc_unreachable();
}

Token Parser::NextToken() {
  Token next_tok = lexer_.Lex();
  cache_ = next_tok;
  return next_tok;
}

bool Parser::TryConsumeToken(TokenKind expected) {
  if (CurrentToken().GetKind() == expected) {
    ConsumeToken();
    return true;
  }
  return false;
}

std::unique_ptr<Type> Parser::ParseTypename() {
  DeclSpec declSpec = ParseDeclSpec();
  return nullptr;
}

void Parser::SkipUntil(TokenKind kind, bool skip_match) {
  while (CurrentToken().GetKind() != kind) {
    ConsumeToken();
  }
  if (skip_match && CurrentToken().GetKind() == kind) {
    ConsumeToken();
  }
}

void Parser::EnterScope() { scopes_.emplace_back(*this); }

void Parser::ExitScope() { scopes_.pop_back(); }

DeclSpec Parser::ParseDeclSpec() {
  using enum TokenKind;
  DeclSpec decl_spec;
  while (CurrentToken().IsTypename()) {
    if (CurrentToken()
            .IsOneOf<Typedef, Static, Extern, Inline, DashThreadLocal>()) {
      // Check if storage class specifier is allowed in this context.
      switch (CurrentToken().GetKind()) {
        case Typedef:
          decl_spec.SetStorageClassSpec(DeclSpec::StorageClassSpec::Typedef);
          break;
        case Static:
          decl_spec.SetStorageClassSpec(DeclSpec::StorageClassSpec::Static);
          break;
        case Extern:
          decl_spec.SetStorageClassSpec(DeclSpec::StorageClassSpec::Extern);
          break;
        case Inline:
          decl_spec.SetFunctionSpec(DeclSpec::FunctionSpec::Inline);
          break;
        case DashThreadLocal:
          decl_spec.SetStorageClassSpec(
              DeclSpec::StorageClassSpec::ThreadLocal);
          break;
        default:
          jcc_unreachable();
      }

      if (decl_spec.IsTypedef()) {
        if (decl_spec.IsStatic() || decl_spec.IsExtern() ||
            decl_spec.IsInline() || decl_spec.IsThreadLocal()) {
          // TODO(Jun): Can you have a nice diag instead of panic?
          jcc_unreachable();
        }
      }
      ConsumeToken();
    }

    // Ignore some keywords with no effects like `auto`
    if (CurrentToken()
            .IsOneOf<Const, Auto, Volatile, Register, Restrict,
                     DashNoReturn>()) {
      ConsumeToken();
    }

    // Deal with _Atomic
    if (CurrentToken().IsOneOf<DashAtmoic>()) {
      ConsumeToken();  // eat `(`
      decl_spec.SetType(ParseTypename());
      ConsumeToken();  // eat `)`
      decl_spec.SetTypeQual(DeclSpec::TypeQual::Atomic);
    }

    // Deal with _Alignas
    if (CurrentToken().Is<TokenKind::DashAlignas>()) {
      jcc_unreachable();
    }

    // Handle user defined types
    if (CurrentToken().IsOneOf<Struct, Union, Typedef, Enum>()) {
      jcc_unreachable();
    }

    // Handle builtin types

    switch (CurrentToken().GetKind()) {
      case Void:
        decl_spec.SetTypeSpecKind(DeclSpec::TSK_Void);
        break;
      case DashBool:
        decl_spec.SetTypeSpecKind(DeclSpec::TSK_Bool);
        break;
      case Char:
        decl_spec.SetTypeSpecKind(DeclSpec::TSK_Char);
        break;
      case Short:
        decl_spec.setTypeSpecWidth(DeclSpec::TypeSpecWidth::Short);
        break;
      case Int:
        decl_spec.SetTypeSpecKind(DeclSpec::TSK_Int);
        break;
      case Long:
        if (decl_spec.GetTypeSpecWidth() == DeclSpec::TypeSpecWidth::Long) {
          decl_spec.setTypeSpecWidth(DeclSpec::TypeSpecWidth::LongLong);
        } else {
          decl_spec.setTypeSpecWidth(DeclSpec::TypeSpecWidth::Long);
        }
        break;
      case Float:
        decl_spec.SetTypeSpecKind(DeclSpec::TSK_Float);
        break;
      case Double:
        decl_spec.SetTypeSpecKind(DeclSpec::TSK_Double);
        break;
      case Signed:
        decl_spec.setTypeSpecSign(DeclSpec::TypeSpecSign::Signed);
        break;
      case Unsigned:
        decl_spec.setTypeSpecSign(DeclSpec::TypeSpecSign::Unsigned);
        break;
      default:
        jcc_unreachable();
    }

    decl_spec.GenerateType();
    ConsumeToken();
  }

  return decl_spec;
}

std::unique_ptr<Type> Parser::ParsePointers(Declarator& declarator) {
  using enum TokenKind;
  std::unique_ptr<Type> type = declarator.GetBaseType();
  ;
  while (TryConsumeToken(Star)) {
    type = Type::CreatePointerType(std::move(type));
    while (CurrentToken().IsOneOf<Const, Volatile, Restrict>()) {
      ConsumeToken();
    }
  }
  return type;
}

std::unique_ptr<Type> Parser::ParseTypeSuffix(std::unique_ptr<Type> type) {
  if (TryConsumeToken(TokenKind::LeftParen)) {
    return ParseParams(std::move(type));
  }
  if (TryConsumeToken(TokenKind::LeftSquare)) {
    return ParseArrayDimensions(std::move(type));
  }
  return type;
}

Declarator Parser::ParseDeclarator(DeclSpec& decl_spec) {
  Declarator declarator(decl_spec);
  std::unique_ptr<Type> type = ParsePointers(declarator);
  if (TryConsumeToken(TokenKind::LeftParen)) {
    DeclSpec dummy;
    ParseDeclarator(dummy);
    ConsumeToken();  // Eat ')'
    std::unique_ptr<Type> suffixType = ParseTypeSuffix(std::move(type));
    DeclSpec suffix;
    suffix.SetType(std::move(suffixType));
    return ParseDeclarator(suffix);
  }

  Token name;
  if (CurrentToken().Is<TokenKind::Identifier>()) {
    name = CurrentToken();
    ConsumeToken();
  }
  // FIXME: Looks like we'll gonna screw up here if the token is not an
  // identifier.
  std::unique_ptr<Type> suffixType = ParseTypeSuffix(std::move(type));
  declarator.name_ = name;
  declarator.setType(std::move(suffixType));
  return declarator;
}

std::unique_ptr<Type> Parser::ParseParams(std::unique_ptr<Type> type) {
  if (CurrentToken().Is<TokenKind::Void>() &&
      NextToken().Is<TokenKind::RightParen>()) {
    SkipUntil(TokenKind::RightParen, /*skip_match=*/true);
    return Type::CreateFuncType(std::move(type));
  }

  std::vector<std::unique_ptr<Type>> params;
  while (!CurrentToken().Is<TokenKind::RightParen>()) {
    std::unique_ptr<Type> type;
    DeclSpec decl_spec = ParseDeclSpec();
    Declarator declarator = ParseDeclarator(decl_spec);

    if (declarator.GetTypeKind() == TypeKind::Array) {
      type = Type::CreatePointerType(
          declarator.GetBaseType()->AsType<PointerType>()->GetBase());
      // FIXME: set name to type.
    } else if (declarator.GetTypeKind() == TypeKind::Func) {
      type = Type::CreatePointerType(declarator.GetBaseType());
    }
    params.emplace_back(std::move(type));
  }

  std::unique_ptr<Type> function_type = Type::CreateFuncType(std::move(type));
  function_type->AsType<FunctionType>()->SetParams(std::move(params));
  return function_type;
}

std::unique_ptr<Type> Parser::ParseArrayDimensions(std::unique_ptr<Type> type) {
  while (CurrentToken().IsOneOf<TokenKind::Static, TokenKind::Restrict>()) {
    ConsumeToken();
  }

  if (TryConsumeToken(TokenKind::RightParen)) {
    std::unique_ptr<Type> arr_type = ParseTypeSuffix(std::move(type));
    return Type::CreateArrayType(std::move(arr_type), -1);
  }

  // cond ? A : B
  // vla
  jcc_unreachable();
}

Expr* Parser::ParseExpr() {
  Expr* expr = ParseAssignmentExpr();
  return ParseRhsOfBinaryExpr(expr);
}

Stmt* Parser::ParseReturnStmt() {
  Expr* return_expr = nullptr;
  if (!TryConsumeToken(TokenKind::Semi)) {
    return_expr = ParseExpr();
    SkipUntil(TokenKind::Semi, /*skip_match=*/true);
  }
  return ReturnStatement::Create(GetASTContext(), SourceRange(), return_expr);
}
Stmt* Parser::ParseStatement() {
  if (TryConsumeToken(TokenKind::Return)) {
    return ParseReturnStmt();
  }

  if (TryConsumeToken(TokenKind::If)) {
    MustConsumeToken(TokenKind::LeftParen);
    Expr* condition = ParseExpr();
    Stmt* then = ParseStatement();
    Stmt* else_stmt = nullptr;
    if (CurrentToken().Is<TokenKind::Else>()) {
      else_stmt = ParseStatement();
    }
    return IfStatement::Create(GetASTContext(), SourceRange(), condition, then,
                               else_stmt);
  }
  jcc_unreachable();
}

std::vector<VarDecl*> Parser::CreateParams(FunctionType* type) {
  std::vector<VarDecl*> params;
  for (std::size_t idx = 0; idx < type->GetParamSize(); idx++) {
    Type* param_type = type->GetParamType(idx);
    params.emplace_back(VarDecl::Create(GetASTContext(), SourceRange(), nullptr,
                                        nullptr, param_type->GetName()));
  }
  return params;
}

Stmt* Parser::ParseCompoundStmt() {
  CompoundStatement* stmt =
      CompoundStatement::Create(GetASTContext(), SourceRange());
  ScopeRAII scope_guard(*this);

  while (!CurrentToken().Is<TokenKind::RightBracket>()) {
    if (CurrentToken().IsTypename() && !NextToken().Is<TokenKind::Colon>()) {
      DeclSpec decl_spec = ParseDeclSpec();
      if (decl_spec.IsTypedef()) {
        // Parse Typedef
        jcc_unreachable();
        continue;
      }
      Declarator declarator = ParseDeclarator(decl_spec);
      if (declarator.GetTypeKind() == TypeKind::Func) {
        stmt->AddStmt(DeclStatement::Create(GetASTContext(), SourceRange(),
                                            ParseFunction(declarator)));
        continue;
      }
      if (decl_spec.IsExtern()) {
        stmt->AddStmt(DeclStatement::Create(GetASTContext(), SourceRange(),
                                            ParseGlobalVariables(declarator)));
        continue;
      }
    } else {
      stmt->AddStmt(ParseStatement());
    }
    // Add type?
  }
  ConsumeToken();  // Eat '}'
  return stmt;
}

Decl* Parser::ParseFunction(Declarator& declarator) {
  Token name = declarator.name_;
  if (!name.IsValid()) {
    jcc_unreachable();
  }
  // Check redefinition

  // FIXME: must capture the type first, or UAF happens!
  std::unique_ptr<Type> base_type = declarator.GetBaseType();
  auto* self = base_type->AsType<FunctionType>();

  ScopeRAII scopeRAII(*this);

  std::vector<VarDecl*> params = CreateParams(self);

  Stmt* body = nullptr;
  if (TryConsumeToken(TokenKind::LeftBracket)) {
    body = ParseCompoundStmt();
  } else if (CurrentToken().Is<TokenKind::Semi>()) {
    // this function doesn't have a body, nothing to do.
  } else {
    jcc_unreachable();
  }

  FunctionDecl* function =
      FunctionDecl::Create(GetASTContext(), SourceRange(), name.GetAsString(),
                           std::move(params), self->GetReturnType(), body);
  return function;
}

std::vector<Decl*> Parser::ParseGlobalVariables(Declarator& declarator) {
  std::vector<Decl*> vars;
  bool is_first = true;
  while (!CurrentToken().Is<TokenKind::Semi>()) {
    if (!is_first) {
      SkipUntil(TokenKind::Comma);
    }
    is_first = false;

    VarDecl* var =
        VarDecl::Create(GetASTContext(), SourceRange(), nullptr,
                        declarator.GetBaseType(), declarator.GetName());
    if (TryConsumeToken(TokenKind::Equal)) {
      var->SetInit(ParseAssignmentExpr());
    }
    vars.push_back(var);
  }
  MustConsumeToken(TokenKind::Semi);  // Eat ';'
  return vars;
}

Expr* Parser::ParseAssignmentExpr() {
  Expr* lhs = ParseCastExpr();

  return ParseRhsOfBinaryExpr(lhs);
}

Expr* Parser::ParseCastExpr() {
  TokenKind kind = CurrentToken().GetKind();
  Expr* result;

  switch (kind) {
    case TokenKind::NumericConstant: {
      int value = std::stoi(CurrentToken().GetData());
      ConsumeToken();
      result = IntergerLiteral::Create(GetASTContext(), SourceRange(), value);
      break;
    }
    default:
      jcc_unreachable();
  }
  return result;
}

Expr* Parser::ParseRhsOfBinaryExpr(Expr* lhs) {
  // TODO(Jun): Not fully implemented!
  return lhs;
}

// Function or a simple declaration
std::vector<Decl*> Parser::ParseFunctionOrVar(DeclSpec& decl_spec) {
  std::vector<Decl*> decls;
  if (CurrentToken().Is<TokenKind::Semi>()) {
    jcc_unreachable();
  }

  Declarator declarator = ParseDeclarator(decl_spec);
  if (declarator.GetTypeKind() == TypeKind::Func) {
    decls.emplace_back(ParseFunction(declarator));
  } else {
    std::vector<Decl*> vars = ParseGlobalVariables(declarator);
    decls.insert(decls.end(), vars.begin(), vars.end());
  }
  return decls;
}

std::vector<Decl*> Parser::ParseTranslateUnit() {
  std::vector<Decl*> topDecls;
  while (!CurrentToken().Is<TokenKind::Eof>()) {
    DeclSpec decl_spec = ParseDeclSpec();
    // TODO(Jun): Handle typedefs
    if (decl_spec.IsTypedef()) {
      jcc_unreachable();
    }

    std::vector<Decl*> decls = ParseFunctionOrVar(decl_spec);
    topDecls.insert(topDecls.end(), decls.begin(), decls.end());
  }

  return topDecls;
}
