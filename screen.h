#ifndef _SCREEN_H
#define _SCREEN_H

#include <stddef.h>

typedef struct area_t {
  size_t origin_x, origin_y;
  size_t rows, columns;
} area_t;

#endif
