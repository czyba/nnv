#include <stdlib.h> //For debugging
#include <alloca.h> //for alloca
#include <string.h> //memset

#include "controller.h" //controller methods
#include "view.h"       //view methods
#include "model.h"      // model methods
#include "termout.h"    // terminal methods

struct controller_t {
  ed_view_t* ed_view;
};

#include <stdio.h>

static void call_back(void* controller, enum CALLBACK_TYPE callback_type) {
  c_t* c = (c_t*) controller;
  if (callback_type == EDITOR_INPUT_ALL || callback_type == EDITOR_INPUT_LINE ||
      callback_type == EDITOR_INPUT_CURSOR) {
    ed_process_input_changed(c->ed_view, callback_type);
  }
}

static void get_area_size(int* rows, int* columns) {
  tcq_t* q = alloc_command_queue(32);
  int olines, ocolumns;
  // Temporarily store our positon
  get_cursor_position(&olines, &ocolumns);
  // Go to the bottom right corner
  append_move_cursor(q, 9999, 9999);
  execute(q);
  reset_queue(q);

  // Get position
  get_cursor_position(rows, columns);

  // Go back to our starting position
  append_move_cursor(q, olines, 1);
  execute(q);
  free_command_queue(q);
}

static void normalize_area(int rows, int columns) {
  tcq_t* q = alloc_command_queue(rows * columns * 2);
  char* a = (char*) alloca(rows * columns);
  memset(a, ' ', rows * columns);
  append_output(q, a, rows * columns);
  append_move_cursor(q, 1, 1);
  execute(q);
  free_command_queue(q);
}

c_t* init_controller() {
  c_t* ret = malloc(sizeof(c_t));
  ed_in_t* in = init_editor_input(&call_back, ret);
  int rows, columns;
  get_area_size(&rows, &columns);
  normalize_area(rows, columns);
  ed_in_load_file(in, "controller.c");
  ret->ed_view = ed_init_editor(in, 1, 1, rows, columns);
  return ret;
}

void free_controller(c_t* c) {
  ed_reset(c->ed_view);
  ed_free(c->ed_view);
  free(c);
}

static void non_ascii_input(ed_in_t* in, key_t k) {
  switch (k.nkey & KEY_MASK) {
  case KEY_UP: {
    ed_in_move_up_line(in);
    break;
  }
  case KEY_DOWN: {
    ed_in_move_down_line(in);
    break;
  }
  case KEY_RIGHT: {
    ed_in_move_forward_character(in);
    break;
  }
  case KEY_LEFT: {
    ed_in_move_back_character(in);
    break;
  }
  case KEY_DEL: {
    ed_in_delete_at_cursor(in);
    break;
  }
  case KEY_HOME: {
    ed_in_move_home(in);
    break;
  }
  case KEY_END: {
    ed_in_move_end(in);
    break;
  }
  }
}

void input_key(c_t* c, key_t k) {
  ed_in_t* in = ed_get_model(c->ed_view);
  if (k.ascii) {
    if (IS_PRINTABLE(k)) {
      ed_in_input_printable_character(in, k);
      return;
    }
    switch (k.key) {
    case ASCII_CR: {
      ed_in_input_LF(in);
      break;
    }
    case ASCII_DEL: {
      if (!ed_in_at_origin(in)) {
        ed_in_move_back_character(in);
        ed_in_delete_at_cursor(in);
      }
      break;
    }
    }
    return;
  }
  non_ascii_input(in, k);
}
