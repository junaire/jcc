#pragma once
#include <fmt/format.h>

#include <cassert>

#if !defined(_MSC_VER)
#define jcc_unreachable()                                            \
  do {                                                               \
    fmt::print("Unexpected error in : {}:{}\n", __FILE__, __LINE__); \
  } while (0)
#else
#define jcc_unreachable() assert(0);
#endif
