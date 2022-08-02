#pragma once

#include <cstdlib>
#include <memory>
#include <vector>

#include "jcc/type.h"

class ASTContext {
  std::vector<void*> slabs_;

  std::unique_ptr<Type> void_;
  std::unique_ptr<Type> bool_;
  std::unique_ptr<Type> char_;
  std::unique_ptr<Type> short_;
  std::unique_ptr<Type> int_;
  std::unique_ptr<Type> long_;
  std::unique_ptr<Type> float_;
  std::unique_ptr<Type> double_;
  std::unique_ptr<Type> ldouble_;

 public:
  ASTContext() { initBuiltinTypes(); };

  template <typename T>
  void* allocate() {
    void* mem = malloc(sizeof(T));
    slabs_.push_back(mem);
    return mem;
  }

  static void deallocate(void* mem) { free(mem); }

  ~ASTContext() {
    for (auto& slab : slabs_) {
      deallocate(slab);
    }
  }

  void initBuiltinTypes() {
#define INIT_TYPE(NAME, KIND, SIZE, ALIGN) \
  NAME = std::make_unique<Type>(KIND, SIZE, ALIGN);

    INIT_TYPE(void_, TypeKind::Void, 2, 1)
    INIT_TYPE(bool_, TypeKind::Bool, 1, 1)
    INIT_TYPE(char_, TypeKind::Char, 1, 1)
    INIT_TYPE(short_, TypeKind::Short, 2, 2)
    INIT_TYPE(int_, TypeKind::Int, 4, 4)
    INIT_TYPE(long_, TypeKind::Long, 8, 8)
    INIT_TYPE(float_, TypeKind::Float, 4, 4)
    INIT_TYPE(double_, TypeKind::Double, 8, 8)
    INIT_TYPE(ldouble_, TypeKind::Ldouble, 16, 16)
  }

  Type* getVoidType() { return void_.get(); }
  Type* getBoolType() { return bool_.get(); }
  Type* getCharType() { return char_.get(); }
  Type* getShortType() { return short_.get(); }
  Type* getIntType() { return int_.get(); }
  Type* getLongType() { return long_.get(); }
  Type* getFloatType() { return float_.get(); }
  Type* getDoubleType() { return double_.get(); }
  Type* getLdoubleType() { return ldouble_.get(); }
};
