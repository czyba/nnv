#include "editor_view.h"
#include "editor_model.h"
#include "termout.h"
#include <stdlib.h>
#include <string.h>
#include <alloca.h>

#include <stdio.h>

/*
 A file is (more or less) a two dimensional array of characters. We assume
 it is a uniform grid. Therefore a file looks more or less like this:



The variables file_x and file_y describe the top left corner of the view
on top of the file. e.g. file_x = 1, file_y = 2 would imply:

  c_1_1 c_1_2 c_1_3 c_1_4
              -------------    <- file_x
  c_2_1 c_2_2 | c_2_3 c_2_4
  c_3_1 c_3_2 | c_3_3 c_3_4
  c_4_1 c_4_2 | c_4_3 c_4_4
              ^
              file_y

Therefore the character in the top left corner of the screen is c_2_3.

The area describes the area of the physical screen we posess. It is similar
to the file view.
*/
struct editor_view_t {
  area_t area;
  // Stores the top right position of the file
  size_t file_x, file_y;
  ed_in_t* in;
};

static void ed_append_line(tcq_t* q, ed_in_t* in, size_t row, size_t column, size_t length) {
  char* a = alloca(length);
  ed_in_fill_line(in, a, row, column, length, ' ');
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
  for (size_t i = 0; i < ed->area.rows; i++) {
    ed_append_line(q, in, ed->file_x + i, ed->file_y, ed->area.columns);
    append_move_cursor(q, ed->area.origin_x + i + 1, ed->area.origin_y);
  }
  append_move_cursor(q, ed->area.origin_x + in_row - ed->file_x, ed->area.origin_y + in_col - ed->file_y);
  execute(q);
  free_command_queue(q);
}

static void ed_move_cursor(ed_view_t* ed) {
  size_t in_row, in_col;
  ed_in_t* in = ed->in;
  tcq_t* q = alloc_command_queue(32);
  ed_in_get_cursor_position(in, &in_row, &in_col);
  append_move_cursor(q, ed->area.origin_x + in_row - ed->file_x, ed->area.origin_y + in_col - ed->file_y);
  execute(q);
  free_command_queue(q);
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

void ed_free(ed_view_t* view) {
  ed_in_free(view->in);
  free(view);
}

area_t ed_get_area(ed_view_t* view) {
  return view->area;
}

void ed_reset(ed_view_t* view) {
  tcq_t* q = alloc_command_queue(view->area.rows * (view->area.columns + 20) + 20);
  char* buf = alloca(view->area.columns);
  memset(buf, ' ', view->area.columns);
  append_options(q, FONT_DEFAULT, NO_OPTION, NO_OPTION);
  for (size_t i = 0; i < view->area.rows; i++) {
    append_move_cursor(q, view->area.origin_x + i, view->area.origin_y);
    append_output(q, buf, view->area.columns);
  }
  append_move_cursor(q, view->area.origin_x, view->area.origin_y);
  execute(q);
  free_command_queue(q);
}

inline ed_in_t* ed_get_model(ed_view_t* view) {
  return view->in;
}

static int ed_adapt_to_input_position(ed_view_t* ed) {
  ed_in_t* in = ed->in;
  size_t in_row, in_col;
  int ret = 0;
  ed_in_get_cursor_position(in, &in_row, &in_col);
  if (ed->file_x > in_row) {
    ed->file_x = in_row;
    ret = 1;
  } else if (ed->file_x + ed->area.rows <= in_row + 1) {
    ed->file_x = in_row - ed->area.rows + 1;
    ret = 1;
  }
  if (ed->file_y > in_col) {
    ed->file_y = in_col;
    ret = 1;
  } else if (ed->file_y + ed->area.columns < in_col + 1) {
    ed->file_y = in_col - ed->area.columns + 1;
    ret = 1;
  }
  return ret;
}

static void ed_process_cursor_changed(ed_view_t* ed) {
  if (ed_adapt_to_input_position(ed)) {
    ed_redraw_everything(ed);
  } else {
    ed_move_cursor(ed);
  }
}

static void ed_process_line_changed(ed_view_t* ed) {
  if (ed_adapt_to_input_position(ed)) {
    ed_redraw_everything(ed);
  } else {
    size_t in_row, in_col;
    ed_in_t* in = ed->in;
    ed_in_get_cursor_position(in, &in_row, &in_col);
    tcq_t* q = alloc_command_queue(ed->area.columns + 64);
    append_move_cursor(q, ed->area.origin_x + in_row - ed->file_x, ed->area.origin_y);
    size_t affected_row = in_row;
    append_options(q, FONT_DEFAULT, FG_BLACK, BG_WHITE);
    ed_append_line(q, in, affected_row, ed->file_y, ed->area.columns);
    append_move_cursor(q, ed->area.origin_x + in_row - ed->file_x, ed->area.origin_y + in_col - ed->file_y);
    execute(q);
    free_command_queue(q);
  }
}

void ed_process_input_changed(ed_view_t* ed, enum CALLBACK_TYPE type) {
  if (type == EDITOR_INPUT_CURSOR) {
    ed_process_cursor_changed(ed);
  } else if (type == EDITOR_INPUT_LINE) {
    ed_process_line_changed(ed);
  } else if (type == EDITOR_INPUT_ALL) {
    ed_adapt_to_input_position(ed);
    ed_redraw_everything(ed);
  }
}

void ed_move_up_screen(ed_view_t* view) {
  ed_in_t* in = view->in;
  size_t in_row, in_col;
  ed_in_get_cursor_position(in, &in_row, &in_col);
  if (in_row - view->area.rows > 0) {
    view->file_x = view->file_x > view->area.rows ? view->file_x > view->area.rows : 0;
  }
  ed_in_move_up_line(in, view->area.rows);
  if (in_row - view->area.rows > 0) {
    ed_redraw_everything(view);
  }
}
void ed_move_down_screen(ed_view_t* view) {
  ed_in_t* in = view->in;
  size_t in_row, in_col, in_num_lines;
  ed_in_get_cursor_position(in, &in_row, &in_col);
  in_num_lines = ed_in_get_num_lines(in);
  if (in_row + view->area.rows < in_num_lines - 1) {
    view->file_x = view->file_x + view->area.rows;
  }
  //Adjust for last page
  if (view->file_x > in_num_lines - 1 - view->area.rows) {
    view->file_x = in_num_lines - view->area.rows;
  }
  ed_in_move_down_line(in, view->area.rows);
  if (in_row + view->area.rows < in_num_lines - 1) {
    ed_redraw_everything(view);
  }
}
