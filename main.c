#include <unistd.h>
#include <stddef.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include "termout_impl.h"
#include <termios.h>
#include <sys/select.h>

#define CHECK if(ret < 0) printf("ERROR HERE: %d", __LINE__);

int main() {
  tcq_t* q = alloc_command_queue(4096 * 3);
  int ret;
  int ix,iy;
  ret = get_cursor_position(&ix,&iy);
  ret = append_move_cursor(q,1,1);
  ret = execute(q);
  sleep(2);
  // ret = reset_queue(q);
  // ret = get_cursor_position(&x,&y);
  // ret = append_move_cursor(q,ix,iy);
  // ret = execute(q);
  // ret = reset_queue(q);
  // ret = append_options(q, FONT_BOLD_ON, NO_OPTION, BG_WHITE);
  // for(int i = 0; i < x * y; i++) {
  //   char c = (char) (i % 26 + 'a');
  //   ret = append_output(q, &c, 1);
  // }
  // sleep(1);
  // execute(q);
  // reset_queue(q);
  // append_move_cursor(q, x / 2, y / 2);
  // execute(q);
  // reset_queue(q);
  // sleep(1);
  // get_cursor_position(&x, &y);
  return ret;
}
