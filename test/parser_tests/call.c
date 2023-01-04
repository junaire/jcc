int foo() { return 42; }

int bar(int x, int y) { return x + y; }

int main() {
  int x = foo();
  x = bar(12, 30);
  return x;
}
