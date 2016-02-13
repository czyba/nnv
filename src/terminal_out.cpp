#include <unistd.h>
#include <terminal_out.h>

using namespace std;
using namespace nnv;

namespace nnv {

terminal_out tout;

terminal_out::terminal_out() {
  fd = 1;
}

size_t terminal_out::write_chars(char const* buf, size_t const start, size_t const end) {
  ssize_t written = write(fd, buf + start, end - start);
  return static_cast<size_t>(written);
}

size_t terminal_out::write_vec(auto_vector<char> const& vec, size_t const start, size_t const end) {
  return write_chars(vec.data(), start, end);
}

void terminal_out::modify_output(enum FONT_OPTIONS option) {
  switch (option) {
    case FONT_OPTIONS::FG_RED:
      write_chars("\x1B[31m",0, 5);
      break;
    default:
      write_chars("\x1B[0m",0, 5);
      break;
  }
}


}
