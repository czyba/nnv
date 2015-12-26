#include "input.h"

static key_t parse_CSI_3(unsigned char* a, size_t len) {
  key_t key;
  key.ascii = 0;
  key.nkey = 0;
  if (len > 4) {
    //TODO:
  }
  if (a[3] == '~') {
    key.nkey = KEY_DEL;
  }
  return key;
}

static key_t parse_CSI_5(unsigned char* a, size_t len) {
  key_t key;
  key.ascii = 0;
  key.nkey = 0;
  if (len > 4) {
    //TODO:
  }
  if (a[3] == '~') {
    key.nkey = KEY_PG_UP;
  }
  return key;
}

static key_t parse_CSI_6(unsigned char* a, size_t len) {
  key_t key;
  key.ascii = 0;
  key.nkey = 0;
  if (len > 4) {
    //TODO:
  }
  if (a[3] == '~') {
    key.nkey = KEY_PG_DOWN;
  }
  return key;
}

static key_t parse_CSI(unsigned char* a, size_t len) {
  key_t key;
  key.ascii = 0;
  key.nkey = 0;
  if (len > 3) {
    switch (a[2]) {
    case '3':
      return parse_CSI_3(a, len);
    case '5':
      return parse_CSI_5(a, len);
    case '6':
      return parse_CSI_6(a, len);
    }
  }
  switch (a[2]) {
  case 'A':
  case 'B':
  case 'C':
  case 'D': {
    key.nkey = a[len - 1] - 'A' + 1;
    return key;
  }
  case 'F': {
    key.nkey = KEY_END;
    break;
  }
  case 'H': {
    key.nkey = KEY_HOME;
    break;
  }
  default:
    break;
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
