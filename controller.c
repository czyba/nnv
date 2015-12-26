#include <stdlib.h> //For debugging
#include <alloca.h> //for alloca
#include <string.h> //memset

#include "controller.h" //controller methods
#include "view.h"       //view methods
#include "model.h"      // model methods
#include "termout.h"    // terminal methods
#include "screen.h"

struct controller_t {
  ed_view_t** ed_view;
  size_t num_files;
  int active_index;
};

#include <stdio.h>

static void call_back(void* controller, enum CALLBACK_TYPE callback_type) {
  c_t* c = (c_t*) controller;
  if (callback_type == EDITOR_INPUT_ALL || callback_type == EDITOR_INPUT_LINE ||
      callback_type == EDITOR_INPUT_CURSOR) {
    ed_process_input_changed(c->ed_view[c->active_index], callback_type);
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
  c_t* c = malloc(sizeof(c_t));
  c->ed_view = 0;
  c->num_files = 0;
  c->active_index = -1;
  int rows, columns;
  get_area_size(&rows, &columns);
  normalize_area(rows, columns);
  return c;
}

void free_controller(c_t* c) {
  for (size_t i = 0; i < c->num_files; i++) {
    ed_reset(c->ed_view[i]);
    ed_free(c->ed_view[i]);
  }
  free(c);
}

void open_file(c_t* c, char* filename) {
  c->num_files++;
  c->ed_view = realloc(c->ed_view, c->num_files * sizeof(ed_view_t*));
  ed_in_t* in = init_editor_input(&call_back, c);
  ed_in_load_file(in, filename);
  int rows, columns;
  get_area_size(&rows, &columns);
  c->ed_view[c->num_files - 1] = ed_init_editor(in, 1, 1, rows, columns);
  c->active_index = c->num_files - 1;
}

static void non_ascii_input(c_t* c, key_t k) {
  ed_view_t* view = c->ed_view[c->active_index];
  ed_in_t* in = ed_get_model(view);
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
    ed_move_up_screen(view);
    // area_t area = ed_get_area(view);
    // ed_in_move_up_line(in, area.rows);
    break;
  }
  case KEY_PG_DOWN: {
    ed_move_down_screen(view);
    break;
  }
  }
}

void input_key(c_t* c, key_t k) {
  ed_in_t* in = ed_get_model(c->ed_view[c->active_index]);
  if (k.ascii) {
    if (IS_PRINTABLE(k)) {
      ed_in_input_printable_character(in, k);
      return;
    }
    switch (k.key) {
    case CTRL_M: {
      ed_in_input_LF(in);
      break;
    }
    case CTRL_N: {
      c->active_index = (c->active_index + 1) % c->num_files;
      ed_process_input_changed(c->ed_view[c->active_index], EDITOR_INPUT_ALL);
      break;
    }
    case CTRL_P: {
      c->active_index = (c->active_index - 1) % c->num_files;
      while (c->active_index < 0) {
        c->active_index += c->num_files;
      }
      ed_process_input_changed(c->ed_view[c->active_index], EDITOR_INPUT_ALL);
      break;
    }
    case CTRL_S: {
      ed_in_save_file(in);
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
