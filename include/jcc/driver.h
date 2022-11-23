#pragma once

#include <filesystem>
#include <string>

namespace jcc {

// Is this a real driver in compiler terminology?
// I don't know, there's real no need to do too much
// abstractions, since we only support Linux and X86 Itanium ABI.
// But it should at least simplify some code, that's good enough :)
class Driver {
  static constexpr const char* default_exe = "a.out";
  std::string executable_name_;
  std::filesystem::path source_file_;
  bool opt_s_ = false;
  bool opt_c_ = false;
  bool opt_o_ = false;

  bool ast_dump_ = false;

 public:
  Driver(int argc, char** argv);
  void Run();

 private:
  void Assemble(const std::string& content, const std::string& source_file,
                bool ast_dump = false);
  void Compile();
  void Link();

  std::string GetObjectName();
  std::string GetAssemblyName();
  std::string GetSourceName();
  std::string GetExeName();
};
}  // namespace jcc
