#ifndef TERMINAL_OUT_H
#define TERMINAL_OUT_H

#include <string>
#include <cstring>

namespace nnv {

class terminal_out {

public:
  terminal_out();
  terminal_out& operator<<(const std::string& s);
  terminal_out& operator<<(const char* s);

protected:
private:
};

extern terminal_out tout;
}

#endif // TERMINAL_OUT_H
