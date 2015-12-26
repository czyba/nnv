#ifndef _CALLBACK_TYPES_H
#define _CALLBACK_TYPES_H

enum CALLBACK_TYPE {
  EDITOR_INPUT_ALL,
  EDITOR_INPUT_LINE,
  EDITOR_INPUT_CURSOR
};

typedef void (*controller_call_back_t)(void*, enum CALLBACK_TYPE);

typedef struct call_back_t {
  void* controller;
  controller_call_back_t cb;
} cb_t;

#endif
