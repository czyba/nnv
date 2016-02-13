#ifndef TERMINAL_OUT_H
#define TERMINAL_OUT_H

#include <vector>
#include <auto_vector.hpp>
#include <terminal_out_options.h>

namespace nnv {

class terminal_out {
public:
  terminal_out();
  ~terminal_out();
  size_t write_vec(auto_vector<char> const& vec, size_t const start, size_t const end);
  bool modify_output(std::vector<enum FONT_OPTIONS> options);

private:
  size_t write_chars(char const* buf, size_t const size);
  int insert_option_code(char* buf, enum FONT_OPTIONS option);
  int fd;
};

extern terminal_out tout;
}

#endif // TERMINAL_OUT_H
