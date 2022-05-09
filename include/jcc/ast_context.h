#pragma once

#include <cstdlib>
#include <vector>

class ASTContext {
  std::vector<void*> slabs;

 public:
  ASTContext() = default;
  void* allocate(std::size_t size) {
    void* mem = malloc(size);
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
