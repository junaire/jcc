#pragma once

#include <cassert>
#include <concepts>
#include <string>
#include <vector>

#include "jcc/source_location.h"

class Expr;
class Decl;
class LabelDecl;

class Stmt {
  SourceRange range_;

 public:
  explicit Stmt(SourceRange rng) : range_(std::move(rng)) {}

  SourceRange getSourceRange() { return range_; }

  template <typename Ty>
  requires std::convertible_to<Ty, Stmt> Ty* asStmt() {
    return static_cast<Ty*>(this);
  }

  virtual ~Stmt() = default;
};

class LabeledStatement : public Stmt {
  LabelDecl* label_;
  Stmt* subStmt_{nullptr};

 public:
  explicit LabeledStatement(SourceRange loc, LabelDecl* label, Stmt* subStmt)
      : Stmt(std::move(loc)), label_(label), subStmt_(subStmt) {}

  Stmt* getSubStmt() { return subStmt_; }
  LabelDecl* getLabel() { return label_; }
};

class CompoundStatement : public Stmt {
  std::vector<Stmt*> stmts_;

 public:
  explicit CompoundStatement(SourceRange loc, std::vector<Stmt*> stmts)
      : Stmt(std::move(loc)), stmts_(std::move(stmts)) {}

  [[nodiscard]] auto getSize() const { return stmts_.size(); }

  Stmt* getStmt(std::size_t index) {
    assert(index < getSize());
    return stmts_[index];
  }
};

class ExpressionStatement : public Stmt {};

class IfStatement : public Stmt {
  Expr* condition_{nullptr};
  Stmt* thenStmt_{nullptr};
  Stmt* elseStmt_{nullptr};

 public:
  IfStatement(SourceRange loc, Expr* condition, Stmt* thenStmt, Stmt* elseStmt)
      : Stmt(std::move(loc)),
        condition_(condition),
        thenStmt_(thenStmt),
        elseStmt_(elseStmt) {}

  Expr* getCondition() { return condition_; }
  Stmt* getThen() { return thenStmt_; }
  Stmt* getElse() { return elseStmt_; }
};

class SwitchStatement : public Stmt {
  std::vector<Stmt*> cases_;

 public:
  explicit SwitchStatement(SourceRange loc, std::vector<Stmt*> cases)
      : Stmt(std::move(loc)), cases_(std::move(cases)) {}

  [[nodiscard]] auto getSize() const { return cases_.size(); }

  Stmt* getStmt(std::size_t index) {
    assert(index < getSize());
    return cases_[index];
  }
};

class WhileStatement : public Stmt {
  Expr* condition_{nullptr};
  Stmt* body_{nullptr};

 public:
  explicit WhileStatement(SourceRange loc, Expr* condition, Stmt* body)
      : Stmt(std::move(loc)), condition_(condition), body_(body) {}

  Expr* getCondition() { return condition_; }
  Stmt* getBody() { return body_; }
};

class DoStatement : public Stmt {
  Expr* condition_{nullptr};
  Stmt* body_{nullptr};

 public:
  explicit DoStatement(SourceRange loc, Expr* condition, Stmt* body)
      : Stmt(std::move(loc)), condition_(condition), body_(body) {}
  Stmt* getBody() { return body_; }
  Expr* getCondition() { return condition_; }
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

  Stmt* getInit() { return init_; }
  Stmt* getCondition() { return condition_; }
  Stmt* getIncrement() { return increment_; }
  Stmt* getBody() { return body_; }
};

class GotoStatement : public Stmt {
  LabelDecl* label_{nullptr};
  SourceRange gotoLoc_;

 public:
  GotoStatement(SourceRange loc, LabelDecl* label, SourceRange gotoLoc)
      : Stmt(std::move(loc)), label_(label), gotoLoc_(std::move(gotoLoc)) {}
};

class ContinueStatement : public Stmt {
  SourceRange continueLoc_;

 public:
  ContinueStatement(SourceRange loc, SourceRange continueLoc)
      : Stmt(std::move(loc)), continueLoc_(std::move(continueLoc)) {}
};

class BreakStatement : public Stmt {
  SourceRange breakLoc_;

 public:
  BreakStatement(SourceRange loc, SourceRange breakLoc)
      : Stmt(std::move(loc)), breakLoc_(std::move(breakLoc)) {}
};

class ReturnStatement : public Stmt {
  Stmt* returnExpr_{nullptr};

 public:
  ReturnStatement(SourceRange loc, Stmt* returnExpr)
      : Stmt(std::move(loc)), returnExpr_(returnExpr) {}
  Stmt* getReturn() { return returnExpr_; }
};

class DeclStmt : public Stmt {
  Decl* decl_{nullptr};

 public:
  DeclStmt(SourceRange loc, Decl* decl) : Stmt(std::move(loc)), decl_(decl) {}

  Decl* getDecl() { return decl_; }
};
