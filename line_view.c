#include "line_view.h"
#include "stdlib.h"
#include "termout.h"

struct line_view_t {
  area_t area;
  ed_view_t* editor;
};

static void print_lines(line_view_t* view) {
  size_t o_row, o_column;
  get_cursor_position(&o_row, &o_column);
  tcq_t* q = alloc_command_queue(view->area.rows * (view->area.columns + 20) + 40);

  execute(q);
  free_command_queue(q);
}

line_view_t* line_init_editor(ed_view_t* editor, size_t origin_x, size_t origin_y, size_t rows, size_t columns){
  line_view_t* view = malloc(sizeof(line_view_t));
  view->editor = editor;
  view->area.origin_x = origin_x;
  view->area.origin_y = origin_y;
  view->area.rows = rows;
  view->area.columns = columns;
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

void line_process_input_changed(line_view_t* view, enum CALLBACK_TYPE type){
  if(type != EDITOR_AREA_CHANGED) {
    return;
  }
  print_lines(view);
}
