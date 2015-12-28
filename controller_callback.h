#ifndef _CALLBACK_TYPES_H
#define _CALLBACK_TYPES_H

enum CALLBACK_TYPE {
  //
  REDRAW,

  //COMMANDS
  EDITOR_CLOSE_TAB,
  EDITOR_NEXT_TAB,
  EDITOR_PREVIOUS_TAB,
  EDITOR_PAGE_UP,
  EDITOR_PAGE_DOWN,

  //
  EDITOR_INPUT_LINE,
  EDITOR_INPUT_CURSOR,

  //
  EDITOR_TOGGLE_LINE_NUMBERING,

  //
  GOTO_LINE_OPEN,
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
