#pragma once

#include <iterator>
#include <map>
#include <string>
#include <type_traits>
#include <vector>

#include "jcc/allocator.h"
#include "jcc/ast_node.h"
#include "jcc/common.h"

namespace jcc {

class Decl;
class FunctionDecl;
class Type;

struct Scope {
  void PushVar(const std::string& name, Decl* var) { vars[name] = var; }
  void PushTag(const std::string& name, Decl* tag) { tags[name] = tag; }

  std::map<std::string, Decl*> vars;
  std::map<std::string, Decl*> tags;
};

class ASTContext {
  std::vector<Type*> user_defined_types_;

  std::vector<Scope> scopes_;

  Allocator<ASTNode> ast_node_allocator_;
  Allocator<Type> type_allocator_;

  FunctionDecl* cur_func_ = nullptr;

 public:
  ASTContext() = default;

  template <typename T>
  void* Allocate() {
    if constexpr (std::is_base_of_v<ASTNode, T>) {
      return ast_node_allocator_.Allocate<T>();
    } else if constexpr (std::is_base_of_v<Type, T>) {
      return type_allocator_.Allocate<T>();
    }
    jcc_unreachable("Can't allocate for unknown type!");
  }

  Type* GetVoidType();
  Type* GetBoolType();
  Type* GetCharType();
  Type* GetShortType();
  Type* GetIntType();
  Type* GetLongType();
  Type* GetUCharType();
  Type* GetUShortType();
  Type* GetUIntType();
  Type* GetULongType();
  Type* GetFloatType();
  Type* GetDoubleType();
  Type* GetLDoubleType();

  void EnterScope() { scopes_.emplace_back(); }
  void ExitScope() { scopes_.pop_back(); }

  Scope& GetCurScope() { return scopes_.back(); }

  // Returns current function we are parseing.
  FunctionDecl* GetCurFunc() { return cur_func_; }

  // TODO(Jun): Look up in vars, need similiar work for tags.
  Decl* Lookup(const std::string& name) {
    for (auto rbeg = scopes_.rbegin(), rend = scopes_.rend(); rbeg != rend;
         rbeg++) {
      auto iter = rbeg->vars.find(name);
      if (iter != rbeg->vars.end()) {
        return iter->second;
      }
    }
    return nullptr;
  }

 private:
  void RegisterUserType(Type* type);
};
}  // namespace jcc
