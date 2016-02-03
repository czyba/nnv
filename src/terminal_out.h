#ifndef TERMINAL_OUT_H
#define TERMINAL_OUT_H

#include <string>
#include <cstring>
#include <terminal_out_options.h>

namespace nnv {

class terminal_out {

public:
  terminal_out();
  terminal_out& operator<<(const std::string& s);
  terminal_out& operator<<(const char* s);
  terminal_out& operator<<(const FONT_OPTIONS val);

protected:
private:
};

extern terminal_out tout;
}

#endif // TERMINAL_OUT_H
