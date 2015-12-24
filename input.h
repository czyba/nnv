#ifndef _INPUT_H
#define _INPUT_H

#include "characters.h"
#include <stddef.h>

#define MOD_NONE  0
#define MOD_SHIFT 0x10000000
#define MOD_CTRL  0x20000000
#define MOD_LALT  0x40000000
#define MOD_RALT  0x80000000

#define KEY_MASK 0x0FFFFFFF
#define UP 0x00000001
#define DOWN 0x00000002
#define RIGHT 0x00000003
#define LEFT 0x00000004

typedef struct key_type_t {
  int ascii;
  union {
    unsigned char key;
    int nkey;
  };
} key_t;

#define IS_PRINTABLE(x) (x.ascii && x.key >= 0x20)

key_t parse_key(unsigned char* sequence, size_t n);

#endif