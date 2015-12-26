#include "tab_view.h"
#include "screen.h"
#include "termout.h"

#include <stdlib.h>
#include <string.h>
#include <alloca.h>

struct tab_view_t {
  area_t area;
  tab_in_t* in;
};

static void insert_characer_times(tcq_t* q, char c, size_t num) {
  char* buf = alloca(num);
  memset(buf, c, num);
  append_output(q, buf, num);
}

static void printf_tabs(tab_view_t* view) {
  tab_in_t* in = view->in;
  char** names = tab_get_names(in);
  size_t num_tabs = tab_get_num_names(view->in);
  size_t index = tab_get_index(view->in);
  size_t cols = view->area.columns;
  //2 rows * cols + num_tabs * 20 (for mode changes) + 50 for backup
  tcq_t* q = alloc_command_queue((cols + 20) * 2 + num_tabs * 20 + 1024);
  append_options(q, NO_OPTION, FG_BLACK, BG_WHITE);
  append_move_cursor(q, view->area.origin_x, view->area.origin_y);
  if (num_tabs == 0) {
    insert_characer_times(q, ' ', cols);
    append_move_cursor(q, view->area.origin_x + 1, view->area.origin_y);
    insert_characer_times(q, '-', cols);
    execute(q);
    free_command_queue(q);
    return;
  }
  size_t total_size = 0;
  for (size_t i = 0; i < num_tabs; i++) {
    total_size += strlen(names[i]);
  }
  if (total_size + num_tabs * 3 - 1 < cols) {
    size_t written_cols = 0;
    for (size_t i = 0; i < num_tabs; i++) {
      char c = ' ';
      size_t len = strlen(names[i]);
      append_output(q, &c, 1);
      if (i == index) {
        append_options(q, FONT_BOLD_ON, FG_RED, BG_WHITE);
        append_output(q, names[i], len);
        append_options(q, FONT_BOLD_OFF, FG_BLACK, BG_WHITE);
      } else {
        append_output(q, names[i], len);
      }
      if (i != num_tabs - 1) {
        written_cols += 2;
        append_output(q, &c, 1);
        c = '|';
        append_output(q, &c, 1);
      }
      written_cols += len + 1;
    }
    insert_characer_times(q, ' ', cols - written_cols);
    append_move_cursor(q, view->area.origin_x + 1, view->area.origin_y);
    insert_characer_times(q, '-', cols - written_cols);
    execute(q);
    free_command_queue(q);
    return;
  }
  free_command_queue(q);
}

tab_view_t* tab_init_editor(tab_in_t* in, int origin_x, int origin_y, int columns) {
  tab_view_t* view = malloc(sizeof(tab_view_t));
  view->area.origin_x = origin_x;
  view->area.origin_y = origin_y;
  view->area.rows = 2;
  view->area.columns = columns;
  view->in = in;
  printf_tabs(view);
  return view;
}
void tab_free(tab_view_t* view) {
  tab_in_free(view->in);
  free(view);
}

tab_in_t* tab_get_model(tab_view_t* view) {
  return view->in;
}

void tab_process_input_changed(tab_view_t* view, enum CALLBACK_TYPE type) {
  if (type != TAB_CHANGED && type != TAB_CLOSED) {
    return;
  }
  (void) view;
  printf_tabs(view);
}
