#include "jcc/parser.h"

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
}

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
    }
    consumeToken();

    if (declSpec.isTypedef()) {
      if (declSpec.isStatic() || declSpec.isExtern() || declSpec.isInline() ||
          declSpec.isThreadLocal()) {
        // TODO(Jun): Can you have a nice diag instead of panic?
        jcc_unreachable();
      }
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
    consumeToken();
  }

  declSpec.constructSelfType();
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
    return nullptr;
  }
  if (currentToken().is<TokenKind::LeftSquare>()) {
    return nullptr;
  }
  return type;
}

Declarator Parser::parseDeclarator(DeclSpec& declSpec) {
  Declarator declarator(declSpec);
  std::unique_ptr<Type> type = parsePointers(declarator);
  if (currentToken().is<TokenKind::LeftParen>()) {
    DeclSpec dummy;
    parseDeclarator(dummy);
    consumeToken();  // Eat ')'
    std::unique_ptr<Type> suffix = parseTypeSuffix(std::move(type));
    // FIXME
  }

  Token name;
  if (currentToken().is<TokenKind::Identifier>()) {
    name = currentToken();
  }
  consumeToken();
  std::unique_ptr<Type> suffixType = parseTypeSuffix(std::move(type));
  declarator.name_ = name;
  declarator.setType(std::move(suffixType));
  return declarator;
}

std::vector<VarDecl*> Parser::parseParams() {
  std::vector<VarDecl*> params;
  do {  // To be implemented
  } while (currentToken().is<TokenKind::Comma>());
  return params;
}

// Create a new scope and parse
Stmt* Parser::parseFunctionBody() { return nullptr; }

Decl* Parser::parseFunction(DeclSpec& declSpec) {
  Declarator declarator = parseDeclarator(declSpec);
  Token name = declarator.name_;
  if (!name.isValid()) {
    jcc_unreachable();
  }
  // Check redefinition
  FunctionDecl* function =
      nullptr; /*FunctionDecl::create(ctx_, SourceRange(), name.getName(), );*/
  return function;
}

std::vector<Decl*> Parser::parseGlobalVariables(DeclSpec& declSpec) {
  std::vector<Decl*> vars;
  while (!currentToken().is<TokenKind::Semi>()) {
    Declarator declarator = parseDeclarator(declSpec);
    vars.emplace_back(nullptr /*VarDecl::create(ctx_, SourceRange(), )*/);
  }
  return vars;
}

// Function or a simple declaration
Decl* Parser::parseDeclaration(DeclSpec& declSpec) {
  // Handle struct-union identifier, like ` enum { X } ;`
  if (currentToken().is<TokenKind::Semi>()) {
  }
  Declarator declarator = parseDeclarator(declSpec);
  if (declarator.getTypeKind() == TypeKind::Func) {
    return parseFunction(declSpec);
  }
  // What should we return?
  return parseGlobalVariables(declSpec)[0];
}

std::vector<Decl*> Parser::parseTranslateUnit() {
  std::vector<Decl*> decls;
  while (!currentToken().is<TokenKind::Eof>()) {
    DeclSpec declSpec = parseDeclSpec();
    // TODO(Jun): Handle typedefs
    if (declSpec.isTypedef()) {
      jcc_unreachable();
    }
    decls.emplace_back(parseDeclaration(declSpec));
  }

  return decls;
}
