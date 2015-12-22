#ifndef _TERMOUT_IMPL_H
#define _TERMOUT_IMPL_H

#include "termout.h"

struct terminal_command_queue_t {
  size_t size;
  size_t pos;
  char* buf;
};

int insert_GENERIC(tcq_t* q, int option, int BOTH_MASK, int OPTION_OFF, char digit);
int append_font_options(tcq_t* q, enum FONT_OPTION font_options);


#endif
