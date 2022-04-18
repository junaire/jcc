#pragma once

#include <string>
#include <vector>

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
  // Clang says we should use a expr to represent the condition
  Stmt* thenStmt_{nullptr};
  Stmt* elseStmt_{nullptr};

 public:
  IfStatement(/*Expr* condition*/ Stmt* thenStmt, Stmt* elseStmt)
      : thenStmt_(thenStmt), elseStmt_(elseStmt) {}

  Stmt* getThen() { return thenStmt_; }
  Stmt* getElse() { return elseStmt_; }
};

class SwitchStatement : public Stmt {
  // Unimplemented
};

class WhileStatement : public Stmt {
  // Clang says we should use a expr to represent the condition
  Stmt* body_{nullptr};

 public:
  explicit WhileStatement(Stmt* body) : body_(body) {}
  Stmt* getBody() { return body_; }
};

class DoStatement : public Stmt {
  Stmt* body_{nullptr};

 public:
  explicit DoStatement(Stmt* body) : body_(body) {}
  Stmt* getBody() { return body_; }
};

class ForStatement : public Stmt {
  Stmt* body_{nullptr};

 public:
  explicit ForStatement(Stmt* body) : body_(body) {}
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
