ParseFirstTopLevelDecl

ParseDeclarationOrFunctionDefinition

  ParseDeclSpec

// A function definition or a group of object declarations
ParseDeclGroup
  ParseDeclarator
    ParseFunctionDeclarator // parse params

ParseFunctionDefinition

ParseFunctionStatementBody
  ParseStatementOrDeclaration


ParseTranslationUnit

| Decl | Decl | Decl | [global variable | function | typedef?]

Declarator Type DeclSpec
