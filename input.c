#include "input.h"

static key_t parse_CSI(unsigned char* a, size_t len) {
  key_t key;
  key.ascii = 0;
  switch(a[len - 1]) {
    case 'A':
    case 'B':
    case 'C':
    case 'D': {
      key.nkey = a[len - 1] - 'A' + 1;
      if(len == 3) {
        break;
      } else if (a[len - 2] == '1') {
        key.nkey = MOD_RALT;
      } else if (a[len - 2] == '3') {
        key.nkey = MOD_LALT;
      } else if (a[len - 2] == '5') {
        key.nkey = MOD_CTRL;
      }
      break;
    }
    default:
      break;
      //TODO
  }
  return key;
}

static key_t parse_ESC(unsigned char* a, size_t len) {
  if(a[1] == '[') {
    return parse_CSI(a, len);
  }
  key_t key;
  key.ascii = 0;
  return key;
}

key_t parse_key(unsigned char* a, size_t len) {
  if(len == 1) {
    key_t key;
    key.ascii = 1;
    key.key = a[0];
    return key;
  }
  if(a[0] == ESC) {
    parse_ESC(a, len);
  }
  key_t key;
  key.ascii = 0;
  return key;
}
