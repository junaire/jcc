#pragma once

#include <string>
#include <vector>

class Expr;

class Stmt {
 public:
  virtual ~Stmt() = default;
};

class LabeledStatement : public Stmt {
  // Clang says we should use a LabelDecl
  Stmt* subStmt_{nullptr};

 public:
  LabeledStatement() = default;
  explicit LabeledStatement(Stmt* subStmt) : subStmt_(subStmt) {}

  Stmt* getSubStmt() { return subStmt_; }
};

class CompoundStatement : public Stmt {
  std::vector<Stmt*> stmts_;

 public:
  explicit CompoundStatement(std::vector<Stmt*> stmts)
      : stmts_(std::move(stmts)) {}
  // add helpers to access stmts.
};

class ExpressionStatement : public Stmt {};

class IfStatement : public Stmt {
  Expr* condition_{nullptr};
  Stmt* thenStmt_{nullptr};
  Stmt* elseStmt_{nullptr};

 public:
  IfStatement(Expr* condition, Stmt* thenStmt, Stmt* elseStmt)
      : condition_(condition), thenStmt_(thenStmt), elseStmt_(elseStmt) {}

  Expr* getCondition() { return condition_; }
  Stmt* getThen() { return thenStmt_; }
  Stmt* getElse() { return elseStmt_; }
};

class SwitchStatement : public Stmt {
  // Unimplemented
};

class WhileStatement : public Stmt {
  Expr* condition_{nullptr};
  Stmt* body_{nullptr};

 public:
  explicit WhileStatement(Expr* condition, Stmt* body)
      : condition_(condition), body_(body) {}
  Expr* getCondition() { return condition_; }
  Stmt* getBody() { return body_; }
};

class DoStatement : public Stmt {
  Expr* condition_{nullptr};
  Stmt* body_{nullptr};

 public:
  explicit DoStatement(Expr* condition, Stmt* body)
      : condition_(condition), body_(body) {}
  Stmt* getBody() { return body_; }
  Expr* getCondition() { return condition_; }
};

class ForStatement : public Stmt {
  Stmt* init_{nullptr};
  Stmt* condition_{nullptr};
  Stmt* increment_{nullptr};
  Stmt* body_{nullptr};

 public:
  explicit ForStatement(Stmt* init, Stmt* condition, Stmt* increment,
                        Stmt* body)
      : init_(init),
        condition_(condition),
        increment_(increment),
        body_(body) {}

  Stmt* getInit() { return init_; }
  Stmt* getCondition() { return condition_; }
  Stmt* getIncrement() { return increment_; }
  Stmt* getBody() { return body_; }
};

class GotoStatement : public Stmt {
  // Unimplemented
};

class ContinueStatement : public Stmt {};

class BreakStatement : public Stmt {};

class ReturnStatement : public Stmt {
  Stmt* returnExpr_{nullptr};

 public:
  explicit ReturnStatement(Stmt* returnExpr) : returnExpr_(returnExpr) {}
  Stmt* getReturn() { return returnExpr_; }
};
