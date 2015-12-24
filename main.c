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

/* Initialize new terminal i/o settings */
static struct termios old, new1;
void initTermios() {
    tcgetattr(0, &old); /* grab old terminal i/o settings */
    new1 = old; /* make new settings same as old settings */
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
  tcq_t* q = alloc_command_queue(4096 * 3);
  int ret;
  int lines,columns;
  ret = append_move_cursor(q,9999,9999);
  ret = execute(q);
  ret = get_cursor_position(&lines,&columns);
  ret = reset_queue(q);
  ret = append_move_cursor(q, 1, 1);
  ret = execute(q);
  ret = reset_queue(q);
  char* a = (char*)alloca(lines * columns);
  memset(a, ' ', lines * columns);
  ret = append_options(q, FONT_BOLD_ON, FG_WHITE, BG_WHITE);
  ret = append_output(q, a, lines * columns);
  ret = append_move_cursor(q, 1, 1);
  ret = execute(q);

  initTermios();

  unsigned char tmp[10];
  ssize_t n;
  while(1) {
    n = read(0, tmp, 10);
    printf("read %lu: ", n);
    for(unsigned int i = 0; i < n; i++) {
      printf("b[%i] = 0x%.2hhx, ", i, tmp[i]);
    }
    printf("\n");
    key_t k = parse_key(tmp, n);
    if(k.ascii && k.key == 0x11) {
      break;
    }
    // append_output(q,tmp, n);
    // execute(q);
    // reset_queue(q);
  }
  resetTermios();

  return ret;
}
