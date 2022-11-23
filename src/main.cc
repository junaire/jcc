#include "jcc/driver.h"

int main(int argc, char** argv) {
  jcc::Driver driver(argc, argv);
  driver.Run();
}
