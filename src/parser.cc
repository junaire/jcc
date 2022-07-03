#include "jcc/parser.h"

Token Parser::consumeToken() { return lexer_.lex(); }

Token Parser::nextToken() {}
