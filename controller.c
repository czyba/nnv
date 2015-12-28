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
#include "goto_model.h"
#include "goto_view.h"

#define CONTROLLER_EDITOR_ACTIVE 0
#define CONTROLLER_GOTO_LINE_ACTIVE 1

struct controller_t {
  tab_in_t* tab_in;
  tab_view_t* tab_view;
  ed_view_t* ed_view;

  goto_in_t* goto_in;
  goto_view_t* goto_view;

  int active_view;
};

static void call_back(void* controller, enum CALLBACK_TYPE callback_type) {
  c_t* c = (c_t*) controller;
  switch (callback_type) {
  case REDRAW:
  case EDITOR_INPUT_LINE:
  case EDITOR_INPUT_CURSOR: {
    ed_process_input_changed(c->ed_view, callback_type);
    break;
  }
  case EDITOR_NEXT_TAB: {
    tab_next(c->tab_in);
    ed_set_model_active(c->ed_view, tab_get_active_tab(c->tab_in));
    break;
  }
  case EDITOR_PREVIOUS_TAB: {
    tab_previous(c->tab_in);
    ed_set_model_active(c->ed_view, tab_get_active_tab(c->tab_in));
    break;
  }
  case EDITOR_PAGE_UP: {
    ed_move_up_screen(c->ed_view);
    break;
  }
  case EDITOR_PAGE_DOWN: {
    ed_move_down_screen(c->ed_view);
    break;
  }
  case EDITOR_TOGGLE_LINE_NUMBERING: {
    ed_process_input_changed(c->ed_view, EDITOR_TOGGLE_LINE_NUMBERING);
    break;
  }
  case EDITOR_CLOSE_TAB: {
    tab_in_unregister_tab(c->tab_in);
    break;
  }
  case TAB_CHANGED: {
    tab_process_input_changed(c->tab_view, callback_type);
    break;
  }
  case TAB_CLOSED: {
    tab_process_input_changed(c->tab_view, callback_type);
    ed_unregister_active_model(c->ed_view);
    ed_set_model_active(c->ed_view, tab_get_active_tab(c->tab_in));
    break;
  }
  case GOTO_LINE_OPEN: {
    c->active_view = CONTROLLER_GOTO_LINE_ACTIVE;
    goto_in_reset(c->goto_in);
    break;
  }
  case GOTO_LINE_CHANGED: {
    goto_process_input_changed(c->goto_view, callback_type);
    break;
  }
  case GOTO_LINE_CLOSE: {
    c->active_view = CONTROLLER_EDITOR_ACTIVE;
    tab_process_input_changed(c->tab_view, REDRAW);
    ed_process_input_changed(c->ed_view, EDITOR_INPUT_CURSOR);
    break;
  }
  case GOTO_LINE_EXECUTE: {
    c->active_view = CONTROLLER_EDITOR_ACTIVE;
    size_t line, col;
    goto_in_get_line_column(c->goto_in, &line, &col);
    tab_process_input_changed(c->tab_view, REDRAW);
    ed_in_goto_position(tab_get_active_tab(c->tab_in), line ? line - 1 : 0, col ? col - 1 : col);
  }
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
  c->active_view = CONTROLLER_EDITOR_ACTIVE;
  c->goto_in = init_goto_in(&call_back, c);
  c->goto_view = goto_init_view(c->goto_in, 1, 1, columns);
  return c;
}

void c_free_controller(c_t* c) {
  ed_free(c->ed_view);
  tab_free(c->tab_view);
  goto_free(c->goto_view);
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

void c_input_key(c_t* c, key_t k) {
  if (c->active_view == CONTROLLER_GOTO_LINE_ACTIVE) {
    goto_in_input_key(c->goto_in, k);
    return;
  } else if (c->active_view == CONTROLLER_EDITOR_ACTIVE) {
    ed_in_t* in = tab_get_active_tab(c->tab_in);
    if (in) {
      ed_in_input_key(in, k);
    }
  }
}
