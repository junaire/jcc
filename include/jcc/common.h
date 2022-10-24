#pragma once
#include <fmt/format.h>

#include <cstdlib>

#if !defined(_MSC_VER)
#define PRETTY_FUNCTION __PRETTY_FUNCTION__
#else
#define PRETTY_FUNCTION __FUNCSIG__
#endif

#define jcc_unreachable(msg)                                   \
  do {                                                         \
    fmt::print("\nUnreachable code executed in: {} ({}:{})\n", \
               PRETTY_FUNCTION, __FILE__, __LINE__);           \
    fmt::print("{}\n", msg);                                   \
    std::abort();                                              \
  } while (0)

#define jcc_unimplemented() jcc_unreachable("Not implemented yet!")
