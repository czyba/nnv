#ifndef TERMINAL_OUT_H
#define TERMINAL_OUT_H

#include <auto_vector.hpp>
#include <terminal_out_options.h>

namespace nnv {

class terminal_out {
public:
  terminal_out();
  size_t write_chars(auto_vector<char> const& vec, size_t const start, size_t const end);

private:
  int fd;
};

extern terminal_out tout;
}

#endif // TERMINAL_OUT_H
