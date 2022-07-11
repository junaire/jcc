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

  [[nodiscard]] std::size_t getOffset() const { return offset_; }

  [[nodiscard]] std::size_t getLine() const { return line_; }

  [[nodiscard]] std::size_t getColumn() const { return column_; }
};

class SourceRange {
  std::string fileName_;
  SourceLocation begin_;
  SourceLocation end_;

 public:
  SourceRange() = default;

  SourceRange(std::string name, const SourceLocation& loc)
      : fileName_(std::move(name)), begin_(loc), end_(loc) {}

  SourceRange(std::string name, const SourceLocation& beg,
              const SourceLocation& end)
      : fileName_(std::move(name)), begin_(beg), end_(end) {}

  void setBegin(const SourceLocation& loc) { begin_ = loc; }

  void setEnd(const SourceLocation& loc) { begin_ = loc; }

  SourceLocation getBegin() { return begin_; }

  SourceLocation getEnd() { return end_; }

  void setFileName(std::string name) { fileName_ = std::move(name); }

  [[nodiscard]] std::string_view getFileName() const { return fileName_; }
};
