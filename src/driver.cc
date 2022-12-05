#include "jcc/driver.h"

#include <fmt/format.h>
#include <unistd.h>
#include <wait.h>

#include <cassert>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <optional>
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
    // keep spinning after the child process is finished.
  }
  if (status != 0) {
    exit(1);
  }
}

static std::filesystem::path GetSourceFile(std::string_view name) {
  std::filesystem::path file(name);
  if (!std::filesystem::exists(file)) {
    fmt::print("No such source file: {}!", name);
  }
  return file;
}

namespace jcc {

// FIXME: Find a better way to deal with the arguments.
Driver::Driver(int argc, char** argv) {
  if (argc < 2) {
    fmt::print("JCC expected aat least 1 argument!\n");
    exit(-1);
  }
  std::vector<std::string_view> args(static_cast<size_t>(argc - 1));
  for (size_t i = 0; i < static_cast<size_t>(argc - 1); ++i) {
    args[i] = argv[i + 1];
  }

  auto iter = args.begin();
  auto end = args.end();
  auto take_arg = [&](auto& arg) {
    if (++arg == end) {
      fmt::print("Expected another argument!\n");
      return false;
    }
    return true;
  };

  while (iter != end) {
    if (*iter == "-o") {
      if (!take_arg(iter)) {
        exit(-1);
      }
      std::filesystem::path source(*iter);
      executable_name_ = source.filename();
      opt_o_ = true;
    } else if (*iter == "-S") {
      opt_s_ = true;
    } else if (*iter == "-c") {
      opt_c_ = true;
    } else if (*iter == "--ast-dump") {
      ast_dump_ = true;
    } else if (iter->starts_with("-")) {
      fmt::print("Unknown argument: {}!\n", *iter);
    } else {
      source_file_ = GetSourceFile(*iter);
    }
    ++iter;
  }
}

void Driver::Run() {
  std::string source_file = GetSourceName();
  std::optional<std::string> contents = ReadFile(source_file);
  if (!contents.has_value()) {
    fmt::print("No such source file: {}!\n", source_file);
    exit(-1);
  }

  if (ast_dump_) {
    Assemble(*contents, GetSourceName(), /*ast-dump*/ true);
    return;
  }
  // Only compile to assembly file.
  if (opt_s_) {
    Assemble(*contents, GetSourceName());
    return;
  }
  // Only compile to object file.
  if (opt_c_) {
    Assemble(*contents, GetSourceName());
    Compile();
    return;
  }
  // Compile to executable.
  if (opt_o_) {
    Assemble(*contents, GetSourceName());
    Compile();
    Link();
    return;
  }
}

// Turn prog.c => prog.s
void Driver::Assemble(const std::string& content,
                      const std::string& source_file, bool ast_dump) {
  Lexer lexer(content, source_file);
  Parser parser(lexer);
  std::vector<Decl*> decls = parser.ParseTranslateUnit();
  if (!ast_dump) {
    GenerateAssembly(source_file, decls);
  } else {
    for (const Decl* decl : decls) {
      decl->dump(0);
    }
  }
}

// Turn prog.s => prog.o
void Driver::Compile() {
  std::string obj_file = GetObjectName();
  std::string asm_file = GetAssemblyName();
  const char* cmd[] = {"as",   "-c", asm_file.c_str(), "-o", obj_file.c_str(),
                       nullptr};
  RunSubprocess(const_cast<char**>(cmd));
}

// Turn prog.o => prog
void Driver::Link() {
  std::string obj_file = GetObjectName();
  std::string exe_file = GetExeName();
  const char* cmd[] = {
      "ld", "-o", exe_file.c_str(), "-m", "elf_x86_64",
      "/usr/lib/x86_64-linux-gnu/crt1.o", "/usr/lib/x86_64-linux-gnu/crti.o",
      "/usr/lib/gcc/x86_64-linux-gnu/11/crtbegin.o",
      "-L/usr/lib/gcc/x86_64-linux-gnu/11", "-L/usr/lib/x86_64-linux-gnu",
      "-L/usr/lib64", "-L/lib64", "-L/usr/lib/x86_64-linux-gnu",
      "-L/usr/lib/x86_64-pc-linux-gnu", "-L/usr/lib/x86_64-redhat-linux",
      "-L/usr/lib", "-L/lib", "-dynamic-linker", "/lib64/ld-linux-x86-64.so.2",
      obj_file.c_str(), "-lc",
      //                     "-lgcc",
      "--as-needed",
      //                     "-lgcc_s",
      "--no-as-needed", "/usr/lib/gcc/x86_64-linux-gnu/11/crtend.o",
      "/usr/lib/x86_64-linux-gnu/crtn.o", nullptr};
  RunSubprocess(const_cast<char**>(cmd));
}

std::string Driver::GetObjectName() {
  std::string stem = std::filesystem::absolute(source_file_).stem();
  return stem + ".o";
}

std::string Driver::GetAssemblyName() {
  std::string stem = std::filesystem::absolute(source_file_).stem();
  return stem + ".s";
}

std::string Driver::GetSourceName() {
  return std::filesystem::absolute(source_file_).string();
}

std::string Driver::GetExeName() {
  std::string dir = std::filesystem::current_path().string();
  if (!executable_name_.empty()) {
    return dir + "/" + executable_name_;
  }
  return dir + "/" + default_exe;
}

}  // namespace jcc
