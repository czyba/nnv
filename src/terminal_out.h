#ifndef TERMINAL_OUT_H
#define TERMINAL_OUT_H

namespace nnv
{

class terminal_out
{
    public:
        terminal_out();
        terminal_out& operator<<(int i);
    protected:
    private:
};

#ifndef NEEDS_TOUT
extern terminal_out tout;
#endif

}

#endif // TERMINAL_OUT_H
