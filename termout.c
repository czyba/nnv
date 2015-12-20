#include "termout.h"
#include <stdlib.h>

struct terminal_command_queue_t {
  size_t size;
  size_t pos;
  char* buf;
};

tcq_t* get_command_queue(size_t size) {
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
