#include "jcc/parser.h"

#include <range/v3/view/concat.hpp>

#include "jcc/common.h"
#include "jcc/decl.h"
#include "jcc/expr.h"
#include "jcc/type.h"

Token Parser::currentToken() { return token_; }

void Parser::consumeToken() {
  if (cache_) {
    token_ = *cache_;
    cache_ = std::nullopt;
  } else {
    token_ = lexer_.lex();
  }
}

Token Parser::nextToken() {
  Token nextTok = lexer_.lex();
  cache_ = nextTok;
  return nextTok;
}

// Returns true if the next token is expected kind, doesn't really consume it.
bool Parser::tryConsumeToken(TokenKind expected) {
  return nextToken().getKind() == expected;
}

std::unique_ptr<Type> Parser::parseTypename() {
  DeclSpec declSpec = parseDeclSpec();
  return nullptr;
}

void Parser::skipUntil(TokenKind kind) {
  while (currentToken().getKind() != kind) {
    consumeToken();
  }
}

void Parser::enterScope() { scopes.emplace_back(*this); }

void Parser::exitScope() { scopes.pop_back(); }

DeclSpec Parser::parseDeclSpec() {
  using enum TokenKind;
  DeclSpec declSpec;
  while (currentToken().isTypename()) {
    if (currentToken()
            .isOneOf<Typedef, Static, Extern, Inline, DashThreadLocal>()) {
      // Check if storage class specifier is allowed in this context.
      switch (currentToken().getKind()) {
        case Typedef:
          declSpec.setStorageClassSpec(DeclSpec::StorageClassSpec::Typedef);
          break;
        case Static:
          declSpec.setStorageClassSpec(DeclSpec::StorageClassSpec::Static);
          break;
        case Extern:
          declSpec.setStorageClassSpec(DeclSpec::StorageClassSpec::Extern);
          break;
        case Inline:
          declSpec.setFunctionSpec(DeclSpec::FunctionSpec::Inline);
          break;
        case DashThreadLocal:
          declSpec.setStorageClassSpec(DeclSpec::StorageClassSpec::ThreadLocal);
          break;
        default:
          jcc_unreachable();
      }

      if (declSpec.isTypedef()) {
        if (declSpec.isStatic() || declSpec.isExtern() || declSpec.isInline() ||
            declSpec.isThreadLocal()) {
          // TODO(Jun): Can you have a nice diag instead of panic?
          jcc_unreachable();
        }
      }
      consumeToken();
    }

    // Ignore some keywords with no effects like `auto`
    if (currentToken()
            .isOneOf<Const, Auto, Volatile, Register, Restrict,
                     DashNoReturn>()) {
      consumeToken();
    }

    // Deal with _Atomic
    if (currentToken().isOneOf<DashAtmoic>()) {
      consumeToken();  // eat `(`
      declSpec.setType(parseTypename());
      consumeToken();  // eat `)`
      declSpec.setTypeQual(DeclSpec::TypeQual::Atomic);
    }

    // Deal with _Alignas
    if (currentToken().is<TokenKind::DashAlignas>()) {
      jcc_unreachable();
    }

    // Handle user defined types
    if (currentToken().isOneOf<Struct, Union, Typedef, Enum>()) {
      jcc_unreachable();
    }

    // Handle builtin types

    switch (currentToken().getKind()) {
      case Void:
        declSpec.setTypeSpecKind(DeclSpec::TSK_Void);
        break;
      case DashBool:
        declSpec.setTypeSpecKind(DeclSpec::TSK_Bool);
        break;
      case Char:
        declSpec.setTypeSpecKind(DeclSpec::TSK_Char);
        break;
      case Short:
        declSpec.setTypeSpecWidth(DeclSpec::TypeSpecWidth::Short);
        break;
      case Int:
        declSpec.setTypeSpecKind(DeclSpec::TSK_Int);
        break;
      case Long:
        if (declSpec.getTypeSpecWidth() == DeclSpec::TypeSpecWidth::Long) {
          declSpec.setTypeSpecWidth(DeclSpec::TypeSpecWidth::LongLong);
        } else {
          declSpec.setTypeSpecWidth(DeclSpec::TypeSpecWidth::Long);
        }
        break;
      case Float:
        declSpec.setTypeSpecKind(DeclSpec::TSK_Float);
        break;
      case Double:
        declSpec.setTypeSpecKind(DeclSpec::TSK_Double);
        break;
      case Signed:
        declSpec.setTypeSpecSign(DeclSpec::TypeSpecSign::Signed);
        break;
      case Unsigned:
        declSpec.setTypeSpecSign(DeclSpec::TypeSpecSign::Unsigned);
        break;
      default:
        jcc_unreachable();
    }

    declSpec.constructSelfType();
    consumeToken();
  }

  return declSpec;
}

std::unique_ptr<Type> Parser::parsePointers(Declarator& declrator) {
  using enum TokenKind;
  std::unique_ptr<Type> type;
  while (currentToken().is<Star>()) {
    consumeToken();
    type = Type::createPointerType(declrator.getBaseType());
    while (currentToken().isOneOf<Const, Volatile, Restrict>()) {
      consumeToken();
    }
  }
  return type;
}

std::unique_ptr<Type> Parser::parseTypeSuffix(std::unique_ptr<Type> type) {
  if (currentToken().is<TokenKind::LeftParen>()) {
    return parseParams(std::move(type));
  }
  if (currentToken().is<TokenKind::LeftSquare>()) {
    return parseArrayDimensions(std::move(type));
  }
  return type;
}

Declarator Parser::parseDeclarator(DeclSpec& declSpec) {
  Declarator declarator(declSpec);
  std::unique_ptr<Type> type = parsePointers(declarator);
  if (currentToken().is<TokenKind::LeftParen>()) {
    consumeToken();
    DeclSpec dummy;
    parseDeclarator(dummy);
    consumeToken();  // Eat ')'
    std::unique_ptr<Type> suffixType = parseTypeSuffix(std::move(type));
    DeclSpec suffix;
    suffix.setType(std::move(suffixType));
    return parseDeclarator(suffix);
  }

  Token name;
  if (currentToken().is<TokenKind::Identifier>()) {
    name = currentToken();
    consumeToken();
  }
  std::unique_ptr<Type> suffixType = parseTypeSuffix(std::move(type));
  declarator.name_ = name;
  declarator.setType(std::move(suffixType));
  return declarator;
}

std::unique_ptr<Type> Parser::parseParams(std::unique_ptr<Type> type) {
  if (currentToken().is<TokenKind::Void>() &&
      nextToken().is<TokenKind::RightParen>()) {
    consumeToken();
    return Type::createFuncType(std::move(type));
  }

  std::vector<std::unique_ptr<Type>> params;
  while (!currentToken().is<TokenKind::RightParen>()) {
    std::unique_ptr<Type> type;
    DeclSpec declSpec = parseDeclSpec();
    Declarator declarator = parseDeclarator(declSpec);

    if (declarator.getTypeKind() == TypeKind::Array) {
      type = Type::createPointerType(
          declarator.getBaseType()->asType<PointerType>()->getBase());
      // FIXME: set name to type.
    } else if (declarator.getTypeKind() == TypeKind::Func) {
      type = Type::createPointerType(declarator.getBaseType());
    }
    params.emplace_back(std::move(type));
  }

  std::unique_ptr<Type> funcType = Type::createFuncType(std::move(type));
  funcType->asType<FunctionType>()->setParams(std::move(params));
  return funcType;
}

std::unique_ptr<Type> Parser::parseArrayDimensions(std::unique_ptr<Type> type) {
  while (currentToken().isOneOf<TokenKind::Static, TokenKind::Restrict>()) {
    consumeToken();
  }

  if (currentToken().is<TokenKind::RightParen>()) {
    consumeToken();
    auto arrType = parseTypeSuffix(std::move(type));
    return Type::createArrayType(std::move(arrType), -1);
  }

  // cond ? A : B
  // vla
  jcc_unreachable();
}

Expr* Parser::parseExpr() { return nullptr; }

Stmt* Parser::parseStatement() {
  if (currentToken().is<TokenKind::Return>()) {
    consumeToken();
    Expr* returnExpr;
    if (currentToken().is<TokenKind::Semi>()) {
      // Return nothing.
      returnExpr = nullptr;
    } else {
      returnExpr = parseExpr();
      // Assume only a semi left.
      consumeToken();
    }

    // Add type?
    return ReturnStatement::create(ctx_, SourceRange(), returnExpr);
  }

  if (currentToken().is<TokenKind::If>()) {
    consumeToken();  // Eat `(`.
    Expr* condition = parseExpr();
    consumeToken();  // Eat `)`.
    Stmt* then = parseStatement();
    Stmt* elseStmt = nullptr;
    if (currentToken().is<TokenKind::Else>()) {
      elseStmt = parseStatement();
    }
    return IfStatement::create(ctx_, SourceRange(), condition, then, elseStmt);
  }
  jcc_unreachable();
}

std::vector<VarDecl*> Parser::createParams(FunctionType* type) {
  std::vector<VarDecl*> params;
  for (std::size_t idx = 0; idx < type->getParamSize(); idx++) {
    Type* paramTy = type->getParamType(idx);
    params.emplace_back(VarDecl::create(ctx_, SourceRange(), nullptr, nullptr,
                                        paramTy->getName()));
  }
  return params;
}

Stmt* Parser::parseCompoundStmt() {
  CompoundStatement* stmt = nullptr;
  ScopeRAII scopeRAII(*this);

  while (!currentToken().is<TokenKind::RightBracket>()) {
    if (currentToken().isTypename() && !nextToken().is<TokenKind::Colon>()) {
      DeclSpec declSpec = parseDeclSpec();
      if (declSpec.isTypedef()) {
        // Parse Typedef
        jcc_unreachable();
        continue;
      }
      Declarator declarator = parseDeclarator(declSpec);
      if (declarator.getTypeKind() == TypeKind::Func) {
        stmt->addStmt(DeclStatement::create(ctx_, SourceRange(),
                                            parseFunction(declarator)));
        continue;
      }
      if (declSpec.isExtern()) {
        stmt->addStmt(DeclStatement::create(ctx_, SourceRange(),
                                            parseGlobalVariables(declSpec)));
        continue;
      }
    } else {
      stmt->addStmt(parseStatement());
    }
    // Add type?
  }
  return stmt;
}

Decl* Parser::parseFunction(Declarator& declarator) {
  Token name = declarator.name_;
  if (!name.isValid()) {
    jcc_unreachable();
  }
  // Check redefinition

  auto* self = declarator.getBaseType()->asType<FunctionType>();

  ScopeRAII scopeRAII(*this);

  std::vector<VarDecl*> params = createParams(self);

  Stmt* body = nullptr;
  if (currentToken().is<TokenKind::Semi>()) {
    consumeToken();
  } else if (currentToken().is<TokenKind::LeftBracket>()) {
    consumeToken();
    body = parseCompoundStmt();
  } else {
    jcc_unreachable();
  }

  FunctionDecl* function =
      FunctionDecl::create(ctx_, SourceRange(), name.getAsString(),
                           std::move(params), self->getReturnType(), body);
  return function;
}

std::vector<Decl*> Parser::parseGlobalVariables(DeclSpec& declSpec) {
  std::vector<Decl*> vars;
  bool isFirst = true;
  while (!currentToken().is<TokenKind::Semi>()) {
    if (!isFirst) {
      skipUntil(TokenKind::Comma);
    }
    isFirst = false;

    Declarator declarator = parseDeclarator(declSpec);
    // propogate some flags from DeclSpec to VarDecl?
    VarDecl* var =
        VarDecl::create(ctx_, SourceRange(), nullptr, declarator.getBaseType(),
                        declarator.getName());
    if (currentToken().is<TokenKind::Equal>()) {
      addInitializer(var);
    }
    vars.push_back(var);
  }
  return vars;
}

void Parser::addInitializer(VarDecl* var) { jcc_unreachable(); }

// Function or a simple declaration
std::vector<Decl*> Parser::parseFunctionOrVar(DeclSpec& declSpec) {
  std::vector<Decl*> decls;
  if (currentToken().is<TokenKind::Semi>()) {
    std::vector<Decl*> vars = parseGlobalVariables(declSpec);
    ranges::views::concat(decls, vars);
    return decls;
  }

  Declarator declarator = parseDeclarator(declSpec);
  if (declarator.getTypeKind() == TypeKind::Func) {
    decls.emplace_back(parseFunction(declarator));
  } else {
    std::vector<Decl*> vars = parseGlobalVariables(declSpec);
    ranges::views::concat(decls, vars);
  }
  return decls;
}

std::vector<Decl*> Parser::parseTranslateUnit() {
  std::vector<Decl*> topDecls;
  while (!currentToken().is<TokenKind::Eof>()) {
    DeclSpec declSpec = parseDeclSpec();
    // TODO(Jun): Handle typedefs
    if (declSpec.isTypedef()) {
      jcc_unreachable();
    }

    std::vector<Decl*> decls = parseFunctionOrVar(declSpec);
    ranges::views::concat(topDecls, decls);
  }

  return topDecls;
}
