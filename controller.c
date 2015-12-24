#include <stdlib.h> //For debugging
#include <alloca.h> //for alloca
#include <string.h> //memset

#include "controller.h" //controller methods
#include "view.h" //view methods
#include "model.h" // model methods
#include "termout.h" // terminal methods

struct controller_t {
  ed_view_t* ed_view;
  ed_in_t* ed_in;

  int rows, columns;
};

#include <stdio.h>

static void call_back (void* controller, enum MODEL_TYPE callback_type) {
  c_t* c = (c_t*) controller;
  if(callback_type == MODEL_EDITOR_INPUT) {
    ed_process_input_changed(c->ed_view);
  }
}

static void get_area_size(int *rows, int* columns){
  tcq_t* q = alloc_command_queue(32);
  int olines,ocolumns;
  //Temporarily store our positon
  get_cursor_position(&olines, &ocolumns);
  //Go to the bottom right corner
  append_move_cursor(q,9999,9999);
  execute(q);
  reset_queue(q);

  //Get position
  get_cursor_position(rows,columns);

  //Go back to our starting position
  append_move_cursor(q, olines, 1);
  execute(q);
  free_command_queue(q);
}

static void normalize_area(int rows, int columns){
  tcq_t* q = alloc_command_queue(rows * columns * 2);
  char* a = (char*)alloca(rows * columns);
  memset(a, ' ', rows * columns);
  append_output(q, a, rows * columns);
  append_move_cursor(q, 1, 1);
  execute(q);
  free_command_queue(q);
}

c_t* init_controller() {
  c_t* ret = malloc(sizeof(c_t));
  ret->ed_in = init_editor_input(&call_back, ret);
  int rows, columns;
  get_area_size(&rows, &columns);
  normalize_area(rows, columns);
  ret->ed_view = ed_init_editor(ret->ed_in, 1, 1, rows, columns);
  return ret;
}

void input_key(c_t* c, key_t key) {
  //Check globals

  //Find active area and notify the respective model
  ed_input(c->ed_in, key);
}
