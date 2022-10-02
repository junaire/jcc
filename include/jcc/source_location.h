#pragma once

#include <cstdio>
#include <string>
#include <string_view>

class SourceLocation {
  std::size_t line_;
  std::size_t column_;
  std::size_t offset_;

 public:
  SourceLocation() = default;
  SourceLocation(std::size_t line, std::size_t column, std::size_t offset)
      : line_(line), column_(column), offset_(offset) {}

  [[nodiscard]] std::size_t GetOffset() const { return offset_; }

  [[nodiscard]] std::size_t GetLine() const { return line_; }

  [[nodiscard]] std::size_t GetColumn() const { return column_; }
};

class SourceRange {
  std::string file_name_;
  SourceLocation begin_;
  SourceLocation end_;

 public:
  SourceRange() = default;

  SourceRange(std::string name, const SourceLocation& loc)
      : file_name_(std::move(name)), begin_(loc), end_(loc) {}

  SourceRange(std::string name, const SourceLocation& beg,
              const SourceLocation& end)
      : file_name_(std::move(name)), begin_(beg), end_(end) {}

  void SetBegin(const SourceLocation& loc) { begin_ = loc; }

  void SetEnd(const SourceLocation& loc) { begin_ = loc; }

  SourceLocation GetBegin() { return begin_; }

  SourceLocation GetEnd() { return end_; }

  void SetFileName(std::string name) { file_name_ = std::move(name); }

  [[nodiscard]] std::string_view GetFileName() const { return file_name_; }
};
