#include <fmt/format.h>

#include <fstream>
#include <string>
#include <string_view>
#include <vector>

#include "jcc/decl.h"
#include "jcc/lexer.h"
#include "jcc/parser.h"

static std::string ReadFile(std::string_view name) {
  std::ifstream file{name.data()};
  std::string contents{std::istreambuf_iterator<char>(file),
                       std::istreambuf_iterator<char>()};
  return contents;
}

int main(int argc, char** argv) {
  if (argc != 2) {
    fmt::print("You should only pass one argument for now!\n");
    return 1;
  }

  std::string fileName = argv[1];
  std::string content = ReadFile(fileName);

  Lexer lexer(content, fileName);
  Parser parser(lexer);

  std::vector<Decl*> decls = parser.ParseTranslateUnit();
  for (const auto* decl : decls) {
    decl->dump(0);
  }
}
