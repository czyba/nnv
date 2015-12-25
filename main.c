#include <unistd.h>
#include <stddef.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include "termout_impl.h"
#include <termios.h>
#include <sys/select.h>
#include <alloca.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include "input.h"
#include "view.h"
#include "model.h"
#include "controller.h"

/* Initialize new terminal i/o settings */
static struct termios old, new1;
void initTermios() {
  tcgetattr(0, &old); /* grab old terminal i/o settings */
  new1 = old;         /* make new settings same as old settings */
  //Disable buffered IO
  new1.c_lflag &= ~(ICANON | ECHO | ECHOK | ECHOE | ECHONL | ISIG | IEXTEN);
  new1.c_iflag |= IGNBRK;
  new1.c_iflag &= ~(ISTRIP | INLCR | ICRNL | IXON | IXOFF);
  tcsetattr(0, TCSANOW, &new1); /* use these new terminal i/o settings now */
}

void resetTermios() {
  tcsetattr(0, TCSANOW, &old); /* resetting terminal */
}

int main() {
  initTermios();

  c_t* c = init_controller();
  (void) c;

  unsigned char tmp[10];
  ssize_t n;
  while (1) {
    n = read(0, tmp, 10);
    key_t k = parse_key(tmp, n);
    if (k.ascii && k.key == 0x11) {
      break;
    }
    input_key(c, k);

    // printf("read %lu: ", n);
    // for(unsigned int i = 0; i < n; i++) {
    //   printf("b[%i] = 0x%.2hhx, ", i, tmp[i]);
    // }
    // printf("\n");
    //
    // printf("ascii = %d, key = %c, nkey = %08x\n", k.ascii, k.key, k.nkey);
  }
  free_controller(c);
  resetTermios();

  return 0;
}
