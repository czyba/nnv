#ifndef _CALLBACK_TYPES_H
#define _CALLBACK_TYPES_H

enum CALLBACK_TYPE {
  EDITOR_INPUT_ALL,
  EDITOR_INPUT_LINE,
  EDITOR_INPUT_CURSOR,
  TAB_CHANGED
};

typedef void (*controller_call_back_t)(void*, enum CALLBACK_TYPE);

typedef struct call_back_t {
  void* controller;
  controller_call_back_t cb;
} cb_t;

void cb_do_callback(cb_t* cb, enum CALLBACK_TYPE type);

#endif
