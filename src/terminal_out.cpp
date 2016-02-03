#include "terminal_out.h"
#include "unistd.h"

using namespace std;

namespace nnv {

terminal_out tout;

terminal_out::terminal_out() {
  // ctor
}

terminal_out& terminal_out::operator<<(String& s) {
  write(1, s.c_str(), s.length(););
  return *this;
}
}
