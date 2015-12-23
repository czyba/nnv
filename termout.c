#include "termout_impl.h"
#include "characters.h"
#include <stdlib.h>

#define CAN_INSERT(q, x) (q->pos + x >= q->size)

int insert_CSI(tcq_t* q) {
  if(q == NULL) {
    return -1;
  }
  if (CAN_INSERT(q,2)) {
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

tcq_t const* alloc_command_queue(size_t size) {
  tcq_t* b = malloc(sizeof(tcq_t));
  if(b == NULL) {
    return b;
  }
  b->buf = malloc(sizeof(size));
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
    INSERT_SEMICOLON_CONDITIONAL(q, ret, origPos)
    written += ret;
  }
  if(IS_BACKGROUND(background_color)) {
    ret = append_color(q, background_color);
    INSERT_SEMICOLON_CONDITIONAL(q, ret, origPos);
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
  memcpy(q->buf,output,n);
}

int append_output_r(tcq_t* q, char* output, size_t n) {
  int written = 0;
  size_t origPos = q->pos;
  int ret = insert_CSI(q);
  if(!CAN_INSERT(q,1)) {
    RESET_AND_RETURN(q, -1, origPos);
  }
  q->buf[q->pos++] = 's';
  written = ret + 1;
  ret = append_output_r(q, output, n);
  RESET_AND_RETURN(q, ret, origPos);
  written += ret;
  ret = insert_CSI(q);
  if(!CAN_INSERT(q,1)) {
    RESET_AND_RETURN(q,-1,origPos);
  }
  q->buf[q->pos++] = 'u'
  written = ret + 1;
  return written;
}

int execute_r(tcq_t* q) {
  //TODO:
}
