#ifndef TERMINAL_OUT_H
#define TERMINAL_OUT_H

#include <auto_vector.hpp>
#include <terminal_out_options.h>

namespace nnv {

class terminal_out {
public:
  terminal_out();
  size_t write_vec(auto_vector<char> const& vec, size_t const start, size_t const end);
  void modify_output(enum FONT_OPTIONS option);
private:
  size_t write_chars(char const* vec, size_t const start, size_t const end);
  int fd;
};

extern terminal_out tout;
}

#endif // TERMINAL_OUT_H
