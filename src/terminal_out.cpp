#define NEEDS_TOUT
#include "terminal_out.h"
#include "unistd.h"

namespace nnv
{

terminal_out tout();

terminal_out::terminal_out()
{
    //ctor
}

terminal_out& terminal_out::operator<<(int x)
{
    char a[3];
    a[0] = 'a';
    a[1] = '\n';
    a[2] = static_cast<char>(x);
    write(1, a, 3);
    return *this;
}

}
