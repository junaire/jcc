#pragma once

#include <cstdlib>
#include <vector>

class ASTContext {
  std::vector<void*> slabs;

 public:
  ASTContext() = default;

  template<typename T>
  void* allocate() {
    void* mem = malloc(sizeof(T));
    slabs.push_back(mem);
    return mem;
  }

  static void deallocate(void* mem) { free(mem); }

  ~ASTContext() {
    for (auto& slab : slabs) {
      deallocate(slab);
    }
  }
};
