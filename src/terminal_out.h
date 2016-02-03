#ifndef TERMINAL_OUT_H
#define TERMINAL_OUT_H

#include <string>

namespace nnv {

class terminal_out {
public:
  terminal_out();
  terminal_out &operator<<(std::string &s);

protected:
private:
};

extern terminal_out tout;
}

#endif // TERMINAL_OUT_H
