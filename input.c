#include "input.h"

static key_t parse_CSI(unsigned char* a, size_t len) {
  key_t key;
  key.ascii = 0;
  key.nkey = 0;
  switch (a[len - 1]) {
  case 'A':
  case 'B':
  case 'C':
  case 'D': {
    key.nkey = a[len - 1] - 'A' + 1;
    if (len != 3) {
      if (a[len - 2] == '1') {
        key.nkey |= MOD_RALT;
      } else if (a[len - 2] == '3') {
        key.nkey |= MOD_LALT;
      } else if (a[len - 2] == '5') {
        key.nkey |= MOD_CTRL;
      }
    }
    return key;
  }
  case 'F': {
    key.nkey = KEY_END;
    return key;
  }
  case 'H': {
    key.nkey = KEY_HOME;
    return key;
  }
  default:
    break;
  }
  if (len == 4 && a[2] == '3' && a[3] == '~') {
    //DEL
    key.nkey = KEY_DEL;
  }
  return key;
}

static key_t parse_ESC(unsigned char* a, size_t len) {
  if (a[1] == '[') {
    return parse_CSI(a, len);
  }
  key_t key;
  key.ascii = 0;
  return key;
}

key_t parse_key(unsigned char* a, size_t len) {
  if (len == 1) {
    key_t key;
    key.ascii = 1;
    key.key = a[0];
    return key;
  }
  if (a[0] == ASCII_ESC) {
    return parse_ESC(a, len);
  }
  key_t key;
  key.ascii = 0;
  return key;
}
