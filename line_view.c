#include "line_view.h"
#include <stdlib.h> //malloc
#include <stddef.h> //NULL
#include <string.h> //memset
#include "termout.h"
#include <alloca.h> //alloca
#include <math.h>   //floor, log10, abs

struct line_view_t {
  area_t area;
  ed_view_t* editor;
  int active;
};

static inline int count_digits(int x) {
  return floor(log10(abs(x))) + 1;
}

static void print_lines(line_view_t* view) {

  size_t start_row, rows;
  int has_input = !ed_get_visible_file_area(view->editor, &start_row, &rows);
  int cols = !view->active ? 0 : count_digits(rows + start_row) + 1;
  if (((size_t) cols) != view->area.columns) {
    size_t old_size = view->area.columns;
    view->area.columns = cols;
    area_t ed_area = ed_get_screen_area(view->editor);
    ed_resize(view->editor, ed_area.origin_x, ed_area.origin_y - old_size + view->area.columns, ed_area.rows, ed_area.columns + old_size - view->area.columns);
    return;
  }
  size_t o_row, o_column;
  get_cursor_position(&o_row, &o_column);
  tcq_t* q = alloc_command_queue(view->area.rows * (view->area.columns + 20) + 40);
  append_move_cursor(q, view->area.origin_x, view->area.origin_y);
  append_options(q, FONT_DEFAULT, FG_BLACK, BG_WHITE);
  char* buf = alloca(view->area.columns);
  buf[view->area.columns - 1] = '|';
  for (size_t i = 1; i <= view->area.rows; i++) {
    size_t tmp = start_row + i;
    int j = view->area.columns - 2;
    if (!has_input || i > rows) {
      memset(buf, ' ', j + 1);
    } else {
      while (tmp && j >= 0) {
        buf[j] = (tmp % 10) + '0';
        tmp /= 10;
        j--;
      }
      while (j >= 0) {
        buf[j] = ' ';
        j--;
      }
    }
    append_output(q, buf, view->area.columns);
    append_move_cursor(q, view->area.origin_x + i, view->area.origin_y);
  }
  append_move_cursor(q, o_row, o_column);
  execute(q);
  free_command_queue(q);
}

line_view_t* line_init_editor(ed_view_t* editor, size_t origin_x, size_t origin_y, size_t rows, size_t columns) {
  line_view_t* view = malloc(sizeof(line_view_t));
  view->editor = editor;
  view->area.origin_x = origin_x;
  view->area.origin_y = origin_y;
  view->area.rows = rows;
  view->area.columns = columns;
  view->active = 1;
  return view;
}

void line_free(line_view_t* view) {
  free(view);
}

void line_resize(line_view_t* view, size_t origin_x, size_t origin_y, size_t rows, size_t columns) {
  view->area.origin_x = origin_x;
  view->area.origin_y = origin_y;
  view->area.rows = rows;
  view->area.columns = columns;
  print_lines(view);
}

void line_process_input_changed(line_view_t* view, enum CALLBACK_TYPE type) {
  if (type != EDITOR_AREA_CHANGED) {
    return;
  }
  print_lines(view);
}
