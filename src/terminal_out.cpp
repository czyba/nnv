#include <unistd.h>
#include <terminal_out.h>

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

terminal_out& terminal_out::operator<<(const FONT_OPTIONS val) {
  char a[10];
  return *this;
}
}
