#include "termout_impl.h"
#include "characters.h"
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <termios.h>
#include <sys/select.h>

#define CAN_INSERT(q, x) (q->pos + x <= q->size)

#define RESET_AND_RETURN(q, ret, restore_pos) \
if(ret < 0) {                   \
  q->buf[restore_pos] = '0';    \
  q->pos = restore_pos;         \
  return -1;                    \
}

#define INSERT_SEMICOLON_CONDITIONAL(q,ret, origPos)  \
if(ret > 0) {                                         \
  if(!CAN_INSERT(q, 1)) {                             \
    RESET_AND_RETURN(q,-1,origPos);                   \
  }                                                   \
  q->buf[q->pos++] = ';';                             \
  ret++;                                              \
}

#define DEFAULT_FONT_APPEND_ACTION(q,ret, origPos)      \
RESET_AND_RETURN(q, ret, origPos);                      \
INSERT_SEMICOLON_CONDITIONAL(q, ret, origPos);          \
written += ret;

inline int count_digits(int x) {
  return floor(log10(abs(x))) + 1;
}

int insert_CSI(tcq_t* q) {
  if(q == NULL) {
    return -1;
  }
  if (!CAN_INSERT(q,2)) {
    return -1;
  }
  q->buf[q->pos++] = ESC;
  q->buf[q->pos++] = '[';
  return 2;
}

int insert_GENERIC(tcq_t* q, int option, int BOTH_MASK, int OPTION_OFF, char digit) {
  if(!(option & BOTH_MASK)) {
    return 0;
  }
  if(q == NULL || ((option & BOTH_MASK) == BOTH_MASK)) {
    //both bits set is nonsensical
    return -1;
  }
  if(option & OPTION_OFF) {
    if(!CAN_INSERT(q, 2)) {
      return -1;
    }
    q->buf[q->pos++] = '2';
    q->buf[q->pos++] = digit;
    return 2;
  }
  if(!CAN_INSERT(q, 1)) {
    return -1;
  }
  q->buf[q->pos++] = digit;
  return 1;
}

tcq_t* alloc_command_queue(size_t size) {
  if(size == 0) {
    return NULL;
  }
  tcq_t* b = malloc(sizeof(tcq_t));
  if(b == NULL) {
    return b;
  }
  b->buf = malloc(size);
  if(!b->buf) {
    free(b);
    b = NULL;
    return b;
  }
  b->size = size;
  b->pos = 0;
  b->buf[0] = 0;
  return b;
}

int free_command_queue(tcq_t* q) {
  if(q == NULL) {
    return 0;
  }
  if(q->buf) {
    free(q->buf);
  }
  free(q);
  return 0;
}

int get_cursor_position(int* line, int* column) {
  struct termios term, initial_term;
  int success = 0;
  fd_set readset;
  tcq_t* q = alloc_command_queue(10);
  if(q == NULL) {
    return -1;
  }
  insert_CSI(q);
  q->buf[q->pos++] = '6';
  q->buf[q->pos++] = 'n';
  /*We store the actual properties of the input console and set it as:
    no buffered (~ICANON): avoid blocking
    no echoing (~ECHO): do not display the result on the console*/
  tcgetattr(STDIN_FILENO, &initial_term);
  term = initial_term;
  term.c_lflag &=~ICANON;
  term.c_lflag &=~ECHO;
  tcsetattr(STDIN_FILENO, TCSANOW, &term);
  //We wait 100ms for a terminal answer
  FD_ZERO(&readset);
  FD_SET(STDIN_FILENO, &readset);
  execute(q);

  //If it success we try to read the cursor value
  if (select(STDIN_FILENO + 1, &readset, NULL, NULL, NULL) == 1) {
    int ret = scanf("\033[%d;%dR", line, column);
    if(ret != 2) {
      success = -1;
    }
  }

  //We set back the properties of the terminal
  tcsetattr(STDIN_FILENO, TCSADRAIN, &initial_term);
  free_command_queue(q);
  return success;
}

#pragma message "Replace by sprintf"
int append_move_cursor(tcq_t* q, int x, int y) {
  if(q == NULL || x <= 0 || y <= 0) {
    return -1;
  }
  size_t origPos = q->pos;
  int written = 0;
  int ret = insert_CSI(q);
  RESET_AND_RETURN(q, ret, origPos)
  written += ret;

  size_t chars = 0;
  int const bufSize = 2 * sizeof(x) + 4;
  char buf[bufSize];
  int pos = bufSize - 1;
  buf[pos--] = 'H';
  chars++;

  while(y != 0) {
    buf[pos--] = (char)((y % 10) + '0');
    y /= 10;
    chars++;
  }
  buf[pos--] = ';';
  chars++;

  while(x != 0) {
    buf[pos--] = (char)((x % 10) + '0');
    x /= 10;
    chars++;
  }

  ret = append_output(q, buf + pos + 1, chars);
  RESET_AND_RETURN(q, ret, origPos);
  written += ret;
  return written;
}

int append_font_options(tcq_t* q, enum FONT_OPTION font_options) {
  size_t origPos = q->pos;
  if(!IS_FONTOPTION(font_options)) {
    return 0;
  }
  if(font_options == FONT_DEFAULT) {
    if(!CAN_INSERT(q,2)) {
      return -1;
    }
    q->buf[q->pos++] = '0';
    q->buf[q->pos++] = ';';
    return 2;
  }
  int ret = 0;
  int written = 0;
  ret = insert_GENERIC(q, font_options & FONT_BOLD_MASK, FONT_BOLD_MASK, FONT_BOLD_OFF, '1');
  DEFAULT_FONT_APPEND_ACTION(q,ret, origPos)

  ret = insert_GENERIC(q, font_options & FONT_BRIGHT_MASK, FONT_BRIGHT_MASK, FONT_BRIGHT_OFF, '2');
  DEFAULT_FONT_APPEND_ACTION(q,ret, origPos)

  ret = insert_GENERIC(q, font_options & FONT_UNDERSCORE_MASK, FONT_UNDERSCORE_MASK, FONT_UNDERSCORE_OFF, '4');
  DEFAULT_FONT_APPEND_ACTION(q,ret, origPos)

  ret = insert_GENERIC(q, font_options & FONT_BLINK_MASK, FONT_BLINK_MASK, FONT_BLINK_OFF, '5');
  DEFAULT_FONT_APPEND_ACTION(q,ret, origPos)
  return written;
}

int append_color(tcq_t* q, unsigned int foreground_color) {
  if(q == NULL) {
    return -1;
  }
  if(!CAN_INSERT(q, 3)) {
    return -1;
  }
  //I want the (char) number, not the actual integer
  q->buf[q->pos++] = (char)(foreground_color / 10) + '0';
  q->buf[q->pos++] = (char)(foreground_color % 10) + '0';
  q->buf[q->pos++] = ';';
  return 3;
}

int append_options(tcq_t* q, enum FONT_OPTION font_options, enum FOREGROUND_COLORS foreground_color, enum BACKGROUND_COLORS background_color){
  if(q == NULL) {
    //No queue given.
    return -1;
  }
  if(font_options == NO_OPTION && foreground_color == NO_OPTION && background_color == NO_OPTION) {
    //nothing to do
    return 0;
  }
  size_t origPos = q->pos;
  int ret = 0;
  int written = 0;
  ret = insert_CSI(q);
  RESET_AND_RETURN(q, ret, origPos);
  written += ret;
  //Order actually shouldn't matter
  if(font_options != NO_OPTION) {
    ret = append_font_options(q, font_options);
    RESET_AND_RETURN(q, ret, origPos);
    written += ret;
  }
  if(IS_FOREGROUND(foreground_color)) {
    ret = append_color(q, foreground_color);
    RESET_AND_RETURN(q, ret, origPos);
    written += ret;
  }
  if(IS_BACKGROUND(background_color)) {
    ret = append_color(q, background_color);
    RESET_AND_RETURN(q, ret, origPos);
    written += ret;
  }
  if(written > 2) {
    q->buf[q->pos-1] = 'm';
  } else {
    q->buf[q->pos++] = 'm';
  }
  return written;
}

int execute(tcq_t* q)  {
  if(q == NULL) {
    return 0;
  }
  int ret = write(1,q->buf,q->pos);
  return ret;
}

int reset_queue(tcq_t* q) {
  if(q == NULL) {
    return 1;
  }
  if(q->size == 0) {
    return 0;
  }
  q->buf[0] = 0;
  q->pos = 0;
  return 0;
}

int append_output(tcq_t* q, char* output, size_t n) {
  if(q == NULL) {
    return -1;
  }
  if(!CAN_INSERT(q,n)) {
    return -1;
  }
  memcpy(q->buf + q->pos, output, n);
  q->pos += n;
  return n;
}

// int append_output_r(tcq_t* q, char* output, size_t n) {
//   if(q == NULL) {
//     return -1;
//   }
//   int written = 0;
//   size_t origPos = q->pos;
//   int ret = insert_CSI(q);
//   if(!CAN_INSERT(q,1)) {
//     RESET_AND_RETURN(q, -1, origPos);
//   }
//   q->buf[q->pos++] = 's';
//   written += ret + 1;
//   ret = append_output(q, output, n);
//   RESET_AND_RETURN(q, ret, origPos);
//   written += ret;
//   ret = insert_CSI(q);
//   if(!CAN_INSERT(q,1)) {
//     RESET_AND_RETURN(q,-1,origPos);
//   }
//   q->buf[q->pos++] = 'u';
//   written += ret + 1;
//   return written;
// }
//
// int execute_r(tcq_t* q) {
//   if(q == NULL || !CAN_INSERT(q,6)) {
//     return -1;
//   }
//   if(q->pos == 0) {
//     return 0;
//   }
//   for(size_t i = q->pos - 1; i > 0; i--) {
//     q->buf[i + 3] = q->buf[i];
//   }
//   q->buf[3] = q->buf[0];
//   size_t origPos = q->pos + 3;
//   q->pos = 0;
//   insert_CSI(q);
//   q->buf[2] = 's';
//   q->pos = origPos;
//   insert_CSI(q);
//   q->buf[q->pos++] = 'u';
//   return execute(q);
// }

int print_queue(tcq_t* queue) {
  for(size_t i = 0; i < queue->pos; i++) {
    char c[2];
    c[0] = queue->buf[i];
    c[1] = 0;
    if(c[0] == ESC) {
      printf("pos = %d, ESC\n", (int)i);
    } else {
      printf("pos = %i, %s\n",(int) i, c);
    }
  }
  return 0;
}
