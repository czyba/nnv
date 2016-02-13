#include <unistd.h>
#include <terminal_out.h>
#include <vector>

#include <iostream>

using namespace std;
using namespace nnv;

namespace nnv {

terminal_out tout;

terminal_out::terminal_out() {
  fd = 1;
}

size_t terminal_out::write_chars(char const* buf, size_t const size) {
  ssize_t written = write(fd, buf, size);
  return static_cast<size_t>(written);
}

size_t terminal_out::write_vec(auto_vector<char> const& vec, size_t const start, size_t const end) {
  return write_chars(vec.data() + start, end - start);
}

int terminal_out::insert_option_code(char* buf, enum FONT_OPTIONS option) {
  return sprintf(buf, "%hhu", option);
}

bool terminal_out::modify_output(std::vector<enum FONT_OPTIONS> options) {
  if (!options.size() || options.size() > 16) {
    return false;
  }
  std::vector<char> buf;
  buf.reserve(3 + options.size() * 3);
  size_t toWrite = 2;
  buf[0] = 0x1B;
  buf[1] = '[';
  for (auto it = options.begin(); it != options.end(); it++) {
    int ret = insert_option_code(buf.data() + toWrite, *it);
    if (ret < 0) {
      return false;
    }
    toWrite += static_cast<size_t>(ret);
    buf[toWrite++] = ';';
  }
  buf[toWrite - 1] = 'm';
  size_t ret = write_chars(buf.data(), toWrite);
  if (ret != toWrite) {
    return false;
  }
  return true;
}
}
