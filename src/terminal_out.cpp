#include <unistd.h>
#include <terminal_out.h>

using namespace std;

namespace nnv {

terminal_out tout;

terminal_out::terminal_out() {
  // ctor
}

size_t terminal_out::write_chars(auto_vector<char> const& vec, size_t const start, size_t const end) {
  ssize_t written = write(1, vec.data() + start, end - start);
  return static_cast<size_t>(written);
}
}
