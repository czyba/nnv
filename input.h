#ifndef _INPUT_H
#define _INPUT_H

#include "characters.h"
#include <inttypes.h>
#include <stddef.h>

#define MOD_NONE 0
#define MOD_SHIFT 0x10000000
#define MOD_CTRL 0x20000000
#define MOD_LALT 0x40000000
#define MOD_RALT 0x80000000

#define KEY_MASK 0x0FFFFFFF
#define KEY_UP 0x00000001
#define KEY_DOWN 0x00000002
#define KEY_RIGHT 0x00000003
#define KEY_LEFT 0x00000004
#define KEY_DEL 0x00000005
#define KEY_HOME 0x00000006
#define KEY_END 0x00000007
#define KEY_PG_UP 0x00000008
#define KEY_PG_DOWN 0x00000009

typedef struct key_type_t {
  int ascii;
  union {
    unsigned char key;
    uint32_t nkey;
  };
} key_t;

#define IS_PRINTABLE(x) (x.ascii && x.key >= 0x20 && x.key != 0x7F)
#define IS_DIGIT(x) (x.ascii && x.key >= '0' && x.key <= '9')

key_t parse_key(unsigned char* sequence, size_t n);

#endif
