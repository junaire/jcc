#pragma once

#include <cstdlib>
#include <vector>
// TODO(Jun): Implement arena based allocator.
template <typename T>
class Allocator {
  std::vector<void*> slabs_;

 public:
  template <typename U>
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
