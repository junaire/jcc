#include <iostream>
#include <string_view>

#include "jcc/lexer.h"
#include "jcc/token.h"

int main() {
  std::string_view code = R"(int main() {return 1;})";
  const char* start = code.data();
  const char* end = code.data() + code.size();
  Lexer lexer{start, end};
  while (!lexer.done()) {
    Token tok = lexer.lex();
    std::cout << tok.getName() << "\n";
  }
}
