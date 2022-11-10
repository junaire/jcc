#include <fmt/format.h>

#include <cstring>
#include <fstream>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "jcc/codegen.h"
#include "jcc/decl.h"
#include "jcc/lexer.h"
#include "jcc/parser.h"

static std::optional<std::string> ReadFile(std::string_view name) {
  std::ifstream file{name.data()};
  std::string contents{std::istreambuf_iterator<char>(file),
                       std::istreambuf_iterator<char>()};
  if (contents.empty()) {
    return std::nullopt;
  }
  return contents;
}

int main(int argc, char** argv) {
  if (argc < 2) {
    fmt::print("Please at least pass one argument to JCC!\n");
    return 1;
  }

  bool ast_dump_mode = false;
  std::string file_name = argv[1];
  std::optional<std::string> content = ReadFile(file_name);
  if (!content.has_value()) {
    fmt::print("No such file!\n");
    return 1;
  }

  // FIXME: Write more robost arg parser for it.
  if (argc == 3) {
    ast_dump_mode = (strcmp(argv[2], "--ast-dump") == 0);
  }

  jcc::Lexer lexer(*content, file_name);
  jcc::Parser parser(lexer);

  std::vector<jcc::Decl*> decls = parser.ParseTranslateUnit();

  if (ast_dump_mode) {
    for (jcc::Decl* decl : decls) {
      decl->dump(0);
    }
  } else {
    jcc::GenerateAssembly(file_name, decls);
  }
}
