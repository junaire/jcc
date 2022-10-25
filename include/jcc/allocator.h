#pragma once

#include <cstdlib>
#include <type_traits>
#include <vector>

namespace jcc {

// TODO(Jun): Implement arena based allocator.
template <typename T>
class Allocator {
  std::vector<void*> slabs_;

 public:
  template <typename U>
  requires std::is_base_of_v<T, U>
  void* Allocate() {
    void* mem = malloc(sizeof(U));
    slabs_.push_back(mem);
    return mem;
  }

  static void Deallocate(void* mem) {
    reinterpret_cast<T*>(mem)->~T();
    free(mem);
  }

  ~Allocator() {
    for (auto* slab : slabs_) {
      Deallocate(slab);
    }
  }
};
}  // namespace jcc
