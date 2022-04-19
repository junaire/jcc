#pragma once
#include "jcc/stmt.h"

// C11 6.5.1
// An expression is a sequence of operators and operands that specifies
// computation of a value, or that designates an object or a function, or that
// generates side effects, or that performs a combination thereof.
class Expr : public Stmt {
 public:
  virtual ~Expr() = default;
};

class StringLiteral : public Expr {
  std::string literal_;

 public:
  explicit StringLiteral(const char* literal) : literal_(literal) {}
};

class CharacterLiteral : public Expr {
  // TODO(Jun): Support more character kinds.
  char value_{0};

 public:
  explicit CharacterLiteral(char value) : value_(value) {}
  [[nodiscard]] char getValue() const { return value_; }
};

class IntergerLiteral : public Expr {
  int value_{0};

 public:
  explicit IntergerLiteral(int value) : value_(value) {}
  [[nodiscard]] int getValue() const { return value_; }
};

class FloatingLiteral : public Expr {
  double value_{0};

 public:
  explicit FloatingLiteral(double value) : value_(value) {}
  [[nodiscard]] double getValue() const { return value_; };
};

class ParenExpr : public Expr {
  Stmt* value_{nullptr};

 public:
  // Clang pass a Expr to the Stmt
  explicit ParenExpr(Expr* stmt) : value_(stmt) {}
  Stmt* getSubExpr() { return value_; }
};

class ConstantExpr : public Expr {
  // C Standard says we need it.
};

class CallExpr : public Expr {
  Expr* callee_{nullptr};
  std::vector<Expr*> args_;

 public:
  explicit CallExpr(Expr* callee, std::vector<Expr*> args)
      : callee_(callee), args_(std::move(args)) {}
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
  explicit UnaryExpr(UnaryOperatorKind kind, Stmt* value)
      : kind_(kind), value_(value) {}

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
  explicit BinaryExpr(BinaryOperatorKind kind, std::vector<Expr*> subExprs)
      : kind_(kind), subExprs_(std::move(subExprs)) {}

  [[nodiscard]] BinaryOperatorKind getKind() const { return kind_; }
};

class ArraySubscriptExpr : public Expr {
  Expr* lhs_{nullptr};
  Expr* rhs_{nullptr};

 public:
  explicit ArraySubscriptExpr(Expr* lhs, Expr* rhs) : lhs_(lhs), rhs_(rhs) {}

  Expr* getLhs() { return lhs_; }
  Expr* getRhs() { return rhs_; }
};

class MemberExpr : public Expr {
  Stmt* base_{nullptr};
  // a decl?
 public:
  explicit MemberExpr(Stmt* base) : base_(base) {}
};
