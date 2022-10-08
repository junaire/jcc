#pragma once

// FIXME: Why can't ASTNode hold source location, we need a better way
// to handle Types.
class ASTNode {
 protected:
  ASTNode() = default;

 public:
  virtual ~ASTNode() = default;
  virtual void dump(int indent) const = 0;
};
