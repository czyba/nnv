#include <stdlib.h> //For debugging
#include <alloca.h> //for alloca
#include <string.h> //memset

#include "controller.h"   //controller methods
#include "editor_view.h"  //view methods
#include "editor_model.h" // model methods
#include "termout.h"      // terminal methods
#include "screen.h"
#include "tab_view.h"
#include "tab_model.h"

struct controller_t {
  tab_in_t* tab_in;
  tab_view_t* tab_view;
  ed_view_t* ed_view;
};

static void call_back(void* controller, enum CALLBACK_TYPE callback_type) {
  c_t* c = (c_t*) controller;
  if (callback_type == EDITOR_INPUT_ALL || callback_type == EDITOR_INPUT_LINE ||
      callback_type == EDITOR_INPUT_CURSOR) {
    ed_process_input_changed(c->ed_view, callback_type);
  } else if (callback_type == TAB_CHANGED) {
    tab_process_input_changed(c->tab_view, callback_type);
  } else if (callback_type == TAB_CLOSED) {
    tab_process_input_changed(c->tab_view, callback_type);
    ed_unregister_active_model(c->ed_view);
    ed_set_model_active(c->ed_view, tab_get_active_tab(c->tab_in));
  }
}

static void get_area_size(size_t* rows, size_t* columns) {
  tcq_t* q = alloc_command_queue(32);
  size_t olines, ocolumns;
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

static void normalize_area(int rows, int columns, int from_origin) {
  tcq_t* q = alloc_command_queue(rows * columns * 2);
  if (from_origin) {
    append_move_cursor(q, 1, 1);
  }
  append_options(q, FONT_DEFAULT, FG_WHITE, BG_BLACK);
  char* a = (char*) alloca(rows * columns);
  memset(a, ' ', rows * columns);
  append_output(q, a, rows * columns);
  append_move_cursor(q, 1, 1);
  execute(q);
  free_command_queue(q);
}

c_t* c_init_controller() {
  c_t* c = malloc(sizeof(c_t));
  c->ed_view = NULL;
  size_t rows, columns;
  get_area_size(&rows, &columns);
  normalize_area(rows, columns, 0);
  c->tab_in = init_tab_input(&call_back, c);
  c->tab_view = tab_init_editor(c->tab_in, 1, 1, columns);
  c->ed_view = ed_init_editor(3, 1, rows - 2, columns);
  return c;
}

void c_free_controller(c_t* c) {
  ed_free(c->ed_view);
  tab_free(c->tab_view);
  size_t rows, columns;
  get_area_size(&rows, &columns);
  normalize_area(rows, columns, 1);
  free(c);
}

void c_open_file(c_t* c, char* filename) {
  if (tab_in_register_tab(c->tab_in, filename)) {
    return;
  }
  ed_register_model(c->ed_view, tab_get_active_tab(c->tab_in));
  ed_set_model_active(c->ed_view, tab_get_active_tab(c->tab_in));
}

static void non_ascii_input(c_t* c, key_t k) {
  ed_in_t* in = tab_get_active_tab(c->tab_in);
  switch (k.nkey & KEY_MASK) {
  case KEY_UP: {
    ed_in_move_up_line(in, 1);
    break;
  }
  case KEY_DOWN: {
    ed_in_move_down_line(in, 1);
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
  case KEY_PG_UP: {
    ed_move_up_screen(c->ed_view);
    break;
  }
  case KEY_PG_DOWN: {
    ed_move_down_screen(c->ed_view);
    break;
  }
  }
}

void c_input_key(c_t* c, key_t k) {
  ed_in_t* in = tab_get_active_tab(c->tab_in);
  if (in == NULL) {
    return;
  }
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
    case CTRL_N: {
      tab_next(c->tab_in);
      ed_set_model_active(c->ed_view, tab_get_active_tab(c->tab_in));
      break;
    }
    case CTRL_P: {
      tab_previous(c->tab_in);
      ed_set_model_active(c->ed_view, tab_get_active_tab(c->tab_in));
      break;
    }
    case CTRL_S: {
      ed_in_save_file(in);
      break;
    }
    case CTRL_W: {
      tab_in_unregister_tab(c->tab_in);
      break;
    }
    case CTRL_L: {
      ed_process_input_changed(c->ed_view, EDITOR_TOGGLE_LINE_NUMBERING);
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
  non_ascii_input(c, k);
}
