#pragma once

#include <cassert>
#include <concepts>
#include <string>
#include <vector>

#include "jcc/source_location.h"

class Expr;
class Decl;
class LabelDecl;
class ASTContext;

class Stmt {
  SourceRange range_;

 public:
  explicit Stmt(SourceRange rng) : range_(std::move(rng)) {}

  SourceRange GetSourceRange() { return range_; }

  template <typename Ty>
  requires std::convertible_to<Ty, Stmt> Ty* AsStmt() {
    return static_cast<Ty*>(this);
  }

  virtual ~Stmt() = default;

  virtual void dump(int indent) const = 0;
};

class LabeledStatement : public Stmt {
  LabelDecl* label_;
  Stmt* sub_stmt_ = nullptr;

 public:
  explicit LabeledStatement(SourceRange loc, LabelDecl* label, Stmt* subStmt)
      : Stmt(std::move(loc)), label_(label), sub_stmt_(subStmt) {}

  Stmt* GetSubStmt() { return sub_stmt_; }
  LabelDecl* GetLabel() { return label_; }
  void dump(int indent) const override;
};

class CompoundStatement : public Stmt {
  std::vector<Stmt*> stmts_;

  explicit CompoundStatement(SourceRange loc) : Stmt(std::move(loc)) {}

 public:
  static CompoundStatement* Create(ASTContext& ctx, SourceRange loc);

  [[nodiscard]] auto GetSize() const { return stmts_.size(); }

  Stmt* GetStmt(std::size_t index) {
    assert(index < GetSize());
    return stmts_[index];
  }

  void AddStmt(Stmt* stmt) { stmts_.emplace_back(stmt); }
  void dump(int indent) const override;
};

class ExpressionStatement : public Stmt {};

class IfStatement : public Stmt {
  Expr* condition_ = nullptr;
  Stmt* then_stmt_ = nullptr;
  Stmt* else_stmt_ = nullptr;

  IfStatement(SourceRange loc, Expr* condition, Stmt* then_stmt,
              Stmt* else_stmt)
      : Stmt(std::move(loc)),
        condition_(condition),
        then_stmt_(then_stmt),
        else_stmt_(else_stmt) {}

 public:
  static IfStatement* Create(ASTContext& ctx, SourceRange loc, Expr* condition,
                             Stmt* thenStmt, Stmt* elseStmt);

  Expr* GetCondition() { return condition_; }
  Stmt* GetThen() { return then_stmt_; }
  Stmt* GetElse() { return else_stmt_; }
  void dump(int indent) const override;
};

class SwitchStatement : public Stmt {
  std::vector<Stmt*> cases_;

 public:
  explicit SwitchStatement(SourceRange loc, std::vector<Stmt*> cases)
      : Stmt(std::move(loc)), cases_(std::move(cases)) {}

  [[nodiscard]] auto GetSize() const { return cases_.size(); }

  Stmt* GetStmt(std::size_t index) {
    assert(index < GetSize());
    return cases_[index];
  }
  void dump(int indent) const override;
};

class WhileStatement : public Stmt {
  Expr* condition_{nullptr};
  Stmt* body_{nullptr};

 public:
  explicit WhileStatement(SourceRange loc, Expr* condition, Stmt* body)
      : Stmt(std::move(loc)), condition_(condition), body_(body) {}

  Expr* GetCondition() { return condition_; }
  Stmt* GetBody() { return body_; }
  void dump(int indent) const override;
};

class DoStatement : public Stmt {
  Expr* condition_{nullptr};
  Stmt* body_{nullptr};

 public:
  explicit DoStatement(SourceRange loc, Expr* condition, Stmt* body)
      : Stmt(std::move(loc)), condition_(condition), body_(body) {}
  Stmt* GetBody() { return body_; }
  Expr* GetCondition() { return condition_; }
  void dump(int indent) const override;
};

class ForStatement : public Stmt {
  Stmt* init_{nullptr};
  Stmt* condition_{nullptr};
  Stmt* increment_{nullptr};
  Stmt* body_{nullptr};

 public:
  explicit ForStatement(SourceRange loc, Stmt* init, Stmt* condition,
                        Stmt* increment, Stmt* body)
      : Stmt(std::move(loc)),
        init_(init),
        condition_(condition),
        increment_(increment),
        body_(body) {}

  Stmt* GetInit() { return init_; }
  Stmt* GetCondition() { return condition_; }
  Stmt* GetIncrement() { return increment_; }
  Stmt* GetBody() { return body_; }
  void dump(int indent) const override;
};

class GotoStatement : public Stmt {
  LabelDecl* label_{nullptr};
  SourceRange goto_loc_;

 public:
  GotoStatement(SourceRange loc, LabelDecl* label, SourceRange goto_loc)
      : Stmt(std::move(loc)), label_(label), goto_loc_(std::move(goto_loc)) {}
  void dump(int indent) const override;
};

class ContinueStatement : public Stmt {
  SourceRange continue_loc_;

 public:
  ContinueStatement(SourceRange loc, SourceRange continue_loc)
      : Stmt(std::move(loc)), continue_loc_(std::move(continue_loc)) {}
  void dump(int indent) const override;
};

class BreakStatement : public Stmt {
  SourceRange breakLoc_;

 public:
  BreakStatement(SourceRange loc, SourceRange breakLoc)
      : Stmt(std::move(loc)), breakLoc_(std::move(breakLoc)) {}
};

class ReturnStatement : public Stmt {
  Expr* return_expr_ = nullptr;

  ReturnStatement(SourceRange loc, Expr* return_expr)
      : Stmt(std::move(loc)), return_expr_(return_expr) {}

 public:
  static ReturnStatement* Create(ASTContext& ctx, SourceRange loc,
                                 Expr* return_expr);
  Expr* GetReturn() { return return_expr_; }
  void dump(int indent) const override;
};

class DeclStatement : public Stmt {
  std::vector<Decl*> decls_{nullptr};
  DeclStatement(SourceRange loc, std::vector<Decl*> decls)
      : Stmt(std::move(loc)), decls_(std::move(decls)) {}
  DeclStatement(SourceRange loc, Decl* decl) : Stmt(std::move(loc)) {
    decls_.emplace_back(decl);
  }

 public:
  static DeclStatement* Create(ASTContext& ctx, SourceRange loc,
                               std::vector<Decl*> decls);
  static DeclStatement* Create(ASTContext& ctx, SourceRange loc, Decl* decl);

  [[nodiscard]] bool IsSingleDecl() const { return decls_.size() == 1; }

  Decl* GetSingleDecl() {
    assert(IsSingleDecl() && "Not a single decl!");
    return decls_[0];
  }

  std::vector<Decl*> GetDecls() { return decls_; }
  void dump(int indent) const override;
};
