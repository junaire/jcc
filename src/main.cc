#include <fmt/format.h>
#include <unistd.h>
#include <wait.h>

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

static void RunSubprocess(char** argv) {
  if (fork() == 0) {
    // Child process. Run a new command.
    execvp(argv[0], argv);
    fprintf(stderr, "exec failed: %s: %s\n", argv[0], strerror(errno));
    _exit(1);
  }
  // Wait for the child process to finish.
  int status;
  while (wait(&status) > 0) {
    ;
  }
  if (status != 0) exit(1);
}

static std::string CreateObjFileName(const std::string& name) {
  assert(name.ends_with(".s") &&
         "Can't generate object file for non assembly!");
  std::string asm_file = name.substr(0, name.size() - 2);
  return asm_file + ".o";
}

static std::string RunAssembler(const std::string& file_name) {
  std::string obj_file = CreateObjFileName(file_name);
  const char* cmd[] = {"as",   "-c", file_name.c_str(), "-o", obj_file.c_str(),
                       nullptr};
  RunSubprocess(const_cast<char**>(cmd));
  return obj_file;
}

static void RunLinker(const std::string& file_name) {
  const char* cmd[] = {
      "ld", "-o", "a.out", "-m", "elf_x86_64",
      "/usr/lib/x86_64-linux-gnu/crt1.o", "/usr/lib/x86_64-linux-gnu/crti.o",
      "/usr/lib/gcc/x86_64-linux-gnu/11/crtbegin.o",
      "-L/usr/lib/gcc/x86_64-linux-gnu/11", "-L/usr/lib/x86_64-linux-gnu",
      "-L/usr/lib64", "-L/lib64", "-L/usr/lib/x86_64-linux-gnu",
      "-L/usr/lib/x86_64-pc-linux-gnu", "-L/usr/lib/x86_64-redhat-linux",
      "-L/usr/lib", "-L/lib", "-dynamic-linker", "/lib64/ld-linux-x86-64.so.2",
      file_name.c_str(), "-lc",
      //                     "-lgcc",
      "--as-needed",
      //                     "-lgcc_s",
      "--no-as-needed", "/usr/lib/gcc/x86_64-linux-gnu/11/crtend.o",
      "/usr/lib/x86_64-linux-gnu/crtn.o", nullptr};
  RunSubprocess(const_cast<char**>(cmd));
}

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
    std::string asm_file = jcc::GenerateAssembly(file_name, decls);
    std::string obj_file = RunAssembler(asm_file);
    RunLinker(obj_file);
  }
}
