#pragma once

#include <memory>

#include "jcc/stmt.h"

class Type;
class Decl;

// C11 6.5.1
// An expression is a sequence of operators and operands that specifies
// computation of a value, or that designates an object or a function, or that
// generates side effects, or that performs a combination thereof.
class Expr : public Stmt {
  std::unique_ptr<Type> type_{nullptr};

 public:
  explicit Expr(SourceRange loc) : Stmt(std::move(loc)) {}
  Expr(SourceRange loc, Type* type) : Stmt(std::move(loc)), type_(type) {}

  Type* getType() { return type_.get(); }
  virtual ~Expr() = default;
};

class StringLiteral : public Expr {
  std::string literal_;

 public:
  StringLiteral(SourceRange loc, const char* literal)
      : Expr(std::move(loc)), literal_(literal) {}
};

class CharacterLiteral : public Expr {
  // TODO(Jun): Support more character kinds.
  char value_{0};

 public:
  CharacterLiteral(SourceRange loc, char value)
      : Expr(std::move(loc)), value_(value) {}
  [[nodiscard]] char getValue() const { return value_; }
};

class IntergerLiteral : public Expr {
  int value_{0};

 public:
  IntergerLiteral(SourceRange loc, int value)
      : Expr(std::move(loc)), value_(value) {}
  [[nodiscard]] int getValue() const { return value_; }
};

class FloatingLiteral : public Expr {
  double value_{0};

 public:
  FloatingLiteral(SourceRange loc, double value)
      : Expr(std::move(loc)), value_(value) {}
  [[nodiscard]] double getValue() const { return value_; };
};

class ConstantExpr : public Expr {
  // C Standard says we need it.
};

class CallExpr : public Expr {
  Expr* callee_{nullptr};
  std::vector<Expr*> args_;

 public:
  CallExpr(SourceRange loc, Expr* callee, std::vector<Expr*> args)
      : Expr(std::move(loc)), callee_(callee), args_(std::move(args)) {}
};

class CastExpr : public Expr {
  // TODO(Jun): Implement this.
};

class InitListExpr : public Expr {
  std::vector<Stmt*> initExprs_;

 public:
  explicit InitListExpr(SourceRange loc, std::vector<Stmt*> initExprs)
      : Expr(std::move(loc)), initExprs_(std::move(initExprs)) {}
};

// TODO(Jun): Add more kinds.
enum class UnaryOperatorKind {
  PreIncrement,
  PostIncrement,
};

class UnaryExpr : public Expr {
  UnaryOperatorKind kind_;
  Stmt* value_{nullptr};

 public:
  UnaryExpr(SourceRange loc, UnaryOperatorKind kind, Stmt* value)
      : Expr(std::move(loc)), kind_(kind), value_(value) {}

  [[nodiscard]] UnaryOperatorKind getKind() const { return kind_; }
};

// TODO(Jun): Add more kinds.
enum class BinaryOperatorKind {
  Plus,
  Minus,
  Multiply,
  Divide,
  Greater,
  Less,
};

class BinaryExpr : public Expr {
  BinaryOperatorKind kind_;
  std::vector<Expr*> subExprs_;

 public:
  BinaryExpr(SourceRange loc, BinaryOperatorKind kind,
             std::vector<Expr*> subExprs)
      : Expr(std::move(loc)), kind_(kind), subExprs_(std::move(subExprs)) {}

  [[nodiscard]] BinaryOperatorKind getKind() const { return kind_; }
};

class ArraySubscriptExpr : public Expr {
  Expr* lhs_{nullptr};
  Expr* rhs_{nullptr};

 public:
  ArraySubscriptExpr(SourceRange loc, Expr* lhs, Expr* rhs)
      : Expr(std::move(loc)), lhs_(lhs), rhs_(rhs) {}

  Expr* getLhs() { return lhs_; }
  Expr* getRhs() { return rhs_; }
};

class MemberExpr : public Expr {
  Stmt* base_{nullptr};
  Decl* member_{nullptr};

 public:
  MemberExpr(SourceRange loc, Stmt* base, Decl* member)
      : Expr(std::move(loc)), base_(base), member_(member) {}

  Stmt* getBase() { return base_; }
  Decl* getMember() { return member_; }
};
