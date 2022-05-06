#pragma once

#include <cstdio>
#include <string>

class SourceLocation {
  std::string filename_;
  std::size_t offset_;

 public:
  SourceLocation() = default;
  SourceLocation(std::string filename, std::size_t offset)
      : filename_(std::move(filename)), offset_(offset) {}

  [[nodiscard]] std::size_t getOffset() const { return offset_; }
  [[nodiscard]] const std::string& getFileName() const { return filename_; }
};

class SourceRange {
  SourceLocation begin_;
  SourceLocation end_;

 public:
  SourceRange() = default;
  explicit SourceRange(SourceLocation loc) : begin_(std::move(loc)), end_(std::move(loc)) {}
  SourceRange(SourceLocation beg, SourceLocation end)
      : begin_(std::move(beg)), end_(std::move(end)) {}

  SourceLocation getBegin() { return begin_; }
  SourceLocation getEnd() { return end_; }
};
