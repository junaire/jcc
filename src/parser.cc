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

Type Parser::parseTypename() { DeclSpec declSpec = parseDeclSpec(); }

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
		if (declSpec.isStatic() || declSpec.isExtern() || declSpec.isInline() || declSpec.isThreadLocal()) {
			// TODO(Jun): Can you have a nice diag instead of panic?
			jcc_unreachable();
		}
	}

  // Ignore some keywords with no effects like `auto`
  if (currentToken()
          .isOneOf<Const, Auto, Volatile, Register, Restrict, DashNoReturn>()) {
    consumeToken();
  }

  // Deal with _Atomic
  if (currentToken().isOneOf<DashAtmoic>()) {
    consumeToken();  // eat `(`
    Type type = parseTypename();
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
  return declSpec;
}

Type Parser::parsePointers(Declarator& declrator) {
  using enum TokenKind;
  Type type;
  while (currentToken().is<Star>()) {
    consumeToken();
    type = Type::createPointerToType(declrator.getType());
    while (currentToken().isOneOf<Const, Volatile, Restrict>()) {
      consumeToken();
    }
  }
  return type;
}

Declarator Parser::parseDeclarator(const DeclSpec& declSpec) {
  Declarator declrator(declSpec);
  Type type = parsePointers(declrator);
  return declrator;
}

std::vector<VarDecl*> Parser::parseParams() {
  std::vector<VarDecl*> params;
  do { // To be implemented
  } while (currentToken().is<TokenKind::Comma>());
  return params;
}

// Create a new scope and parse
Stmt* Parser::parseFunctionBody() { return nullptr; }

Decl* Parser::parseFunction(const DeclSpec& declSpec) {
  Declarator declrator = parseDeclarator(declSpec);
  // Check redefinition
}

// Function or a simple declaration
Decl* Parser::parseDeclaration(const DeclSpec& declSpec) {
  // Handle struct-union identifier, like ` enum { X } ;`
  if (currentToken().is<TokenKind::Semi>()) {
  }
  Declarator declrator = parseDeclarator(declSpec);
}

std::vector<Decl*> Parser::parseTranslateUnit() {
  std::vector<Decl*> decls;
  while (!currentToken().is<TokenKind::Eof>()) {
    DeclSpec declSpec = parseDeclSpec();
    // TODO(Jun): Handle typedefs
    if (declSpec.isTypedef()) {
      jcc_unreachable();
    }

    // Parse functions
    // Parse global variables
  }

  return decls;
}
