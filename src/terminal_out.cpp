#include "terminal_out.h"
#include "unistd.h"

using namespace std;

namespace nnv {

terminal_out tout;

terminal_out::terminal_out() {
  // ctor
}

terminal_out& terminal_out::operator<<(const string& s) {
  write(1, s.c_str(), s.length());
  return *this;
}

terminal_out& terminal_out::operator<<(const char* s) {
  write(1, s, strlen(s));
  return *this;
}
}
