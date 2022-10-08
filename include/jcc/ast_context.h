#pragma once

#include <cstdlib>
#include <vector>

class Type;
#include "jcc/ast_node.h"

class ASTContext {
  // TODO(Jun): Implement arena based allocator.
  // TODO(Jun): Seperate the allocator from ASTContext, make it more generic.
  std::vector<void*> slabs_;

  std::vector<Type*> user_defined_types_;

  Type* void_type_;
  Type* bool_type_;

  Type* char_type_;
  Type* short_type_;
  Type* int_type_;
  Type* long_type_;

  Type* uchar_type_;
  Type* ushort_type_;
  Type* uint_type_;
  Type* ulong_type_;

  Type* float_type_;
  Type* double_type_;
  Type* ldouble_type_;

 public:
  ASTContext();

  template <typename T>
  void* Allocate() {
    void* mem = malloc(sizeof(T));
    slabs_.push_back(mem);
    return mem;
  }

  static void Deallocate(void* mem) {
    reinterpret_cast<ASTNode*>(mem)->~ASTNode();
    free(mem);
  }

  ~ASTContext() {
    for (auto& slab : slabs_) {
      Deallocate(slab);
    }
  }

  // TODO(Jun): signedness, long long ...
  Type* GetVoidType() { return void_type_; }
  Type* GetBoolType() { return bool_type_; }

  Type* GetCharType() { return char_type_; }
  Type* GetShortType() { return short_type_; }
  Type* GetIntType() { return int_type_; }
  Type* GetLongType() { return long_type_; }

  Type* GetUCharType() { return char_type_; }
  Type* GetUShortType() { return short_type_; }
  Type* GetUIntType() { return int_type_; }
  Type* GetULongType() { return long_type_; }

  Type* GetFloatType() { return float_type_; }
  Type* GetDoubleType() { return double_type_; }

 private:
  void CreateBuiltinTypes();
  void RegisterUserType(Type* type);
};
