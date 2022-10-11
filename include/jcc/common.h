#pragma once
#include <fmt/format.h>

#include <cstdlib>

#define jcc_unreachable()                                      \
  do {                                                         \
    fmt::print("\nUnreachable code executed in: {} ({}:{})\n", \
               __PRETTY_FUNCTION__, __FILE__, __LINE__);       \
    std::abort();                                              \
  } while (0)
