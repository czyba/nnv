#include "goto_view.h"
#include "goto_model.h"
#include "screen.h"
#include "termout.h"
#include "basic_math.h"
#include <stdlib.h>
#include <alloca.h>
#include <string.h>

#define START_STRING "Insert Line:Column : "

struct goto_view_t {
  area_t area;
  goto_in_t* in;
  size_t in_y;
};

static void goto_print_line(goto_view_t* view) {
  tcq_t* q = alloc_command_queue(view->area.rows * (20 + view->area.columns) + 40);
  append_move_cursor(q, view->area.origin_y, view->area.origin_y);
  append_options(q, FONT_DEFAULT, FG_BLACK, BG_BLUE);
  size_t pos = goto_in_get_pos(view->in);
  if (pos < view->in_y) {
    view->in_y = pos;
  }
  if (pos >= view->in_y + view->area.columns) {
    view->in_y = pos - view->area.columns;
  }
  char* buf = alloca(view->area.columns);
  size_t written = minu(strlen(START_STRING), view->area.columns);
  memcpy(buf, START_STRING, written);
  if (written < view->area.columns) {
    goto_in_fill_line(view->in, buf, written, view->area.columns - written, ' ');
  }
  append_output(q, buf, view->area.columns);
  append_move_cursor(q, view->area.origin_x, view->area.origin_y + strlen(START_STRING) + pos - view->in_y);
  execute(q);
  free_command_queue(q);
}

goto_view_t* goto_init_view(goto_in_t* in, size_t origin_x, size_t origin_y, size_t columns) {
  goto_view_t* view = malloc(sizeof(goto_view_t));
  view->area.origin_x = origin_x;
  view->area.origin_y = origin_y;
  view->area.rows = 1;
  view->area.columns = columns;
  view->in = in;
  view->in_y = 0;
  return view;
}

void goto_free(goto_view_t* view) {
  free_goto_in(view->in);
  free(view);
}

void goto_process_input_changed(goto_view_t* view, enum CALLBACK_TYPE type) {
  if (type != GOTO_LINE_CHANGED) {
    return;
  }
  goto_print_line(view);
}
