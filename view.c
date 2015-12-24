#include "view.h"
#include "model.h"
#include "termout.h"
#include <stdlib.h>
#include <string.h>
#include <alloca.h>

#include <stdio.h>

typedef struct area_t {
  size_t origin_x, origin_y;
  size_t rows, columns;
} area_t;

struct editor_view_t {
  area_t area;
  // Stores the top right position of the file
  size_t file_x, file_y;
  ed_in_t* in;
};

static void ed_append_line(tcq_t* q, ed_in_t* in, size_t row, size_t column, size_t length) {
  char* a = alloca(length);
  fill_line(in, a, row, column, length, ' ');
  append_output(q, a, length);
}

static void ed_redraw_everything(ed_view_t* ed) {
  size_t in_row, in_col;
  ed_in_t* in = ed->in;
  ed_in_get_cursor_position(in, &in_row, &in_col);
  #pragma message "heuristic is for small terminals SHIT"
  tcq_t* q = alloc_command_queue(ed->area.rows * ed->area.columns * 2);
  append_move_cursor(q, ed->area.origin_x, ed->area.origin_y);
  append_options(q, FONT_DEFAULT, FG_BLACK, BG_WHITE);
  for(size_t i = 0; i < ed->area.rows; i++) {
    ed_append_line(q, in, ed->file_x + i, ed->file_y, ed->area.columns);
    append_move_cursor(q, ed->area.origin_x + i + 1, ed->area.origin_y);
  }
  append_move_cursor(q,  ed->area.origin_x + in_row - ed->file_x, ed->area.origin_y + in_col - ed->file_y);
  execute(q);
  free_command_queue(q);
  (void) in;

}

ed_view_t* ed_init_editor(ed_in_t* in, int origin_x, int origin_y, int rows, int columns) {
  ed_view_t* ed = malloc(sizeof(ed_view_t));
  ed->in = in;
  ed->area.origin_x = origin_x;
  ed->area.origin_y = origin_y;
  ed->area.rows = rows;
  ed->area.columns = columns;
  ed->file_x = 0;
  ed->file_y = 0;
  ed_redraw_everything(ed);
  return ed;
}

void ed_process_input_changed(ed_view_t* ed, enum CALLBACK_TYPE type){
  (void) type;
  ed_in_t* in = ed->in;
  size_t in_row, in_col;
  ed_in_get_cursor_position(in, &in_row, &in_col);
  if(ed->file_x > in_row) {
    ed->file_x = in_row;
  }
  if(ed->file_x + ed->area.rows <= in_row + 1) {
    ed->file_x = in_row - ed->area.rows + 1;
  }
  if(ed->file_y > in_col) {
    ed->file_y = in_col;
  }
  if(ed->file_y + ed->area.columns < in_col + 1) {
    ed->file_y = in_col - ed->area.columns + 1;
  }
  ed_redraw_everything(ed);
}
