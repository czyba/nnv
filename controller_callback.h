#ifndef _CALLBACK_TYPES_H
#define _CALLBACK_TYPES_H

enum CALLBACK_TYPE {
  REDRAW,
  EDITOR_INPUT_LINE,
  EDITOR_INPUT_CURSOR,
  EDITOR_AREA_CHANGED,
  EDITOR_TOGGLE_LINE_NUMBERING,
  GOTO_LINE_CHANGED,
  GOTO_LINE_EXECUTE,
  GOTO_LINE_CLOSE,
  TAB_CHANGED,
  TAB_CLOSED
};

typedef void (*controller_call_back_t)(void*, enum CALLBACK_TYPE);

typedef struct call_back_t {
  void* controller;
  controller_call_back_t cb;
} cb_t;

void cb_do_callback(cb_t* cb, enum CALLBACK_TYPE type);

#endif
