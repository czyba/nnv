#include "editor_view.h"
#include "editor_model.h"
#include "termout.h"
#include "line_view.h"
#include "basic_math.h"
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
typedef struct input_ref {
  // Stores the top left position of the file
  size_t file_x, file_y;
  ed_in_t* in;
} i_ref_t;

struct editor_view_t {
  area_t area;
  i_ref_t* refs;
  size_t active;
  size_t num_refs;
  line_view_t* line_view;
};

static i_ref_t* ed_get_active_ref(ed_view_t* ed) {
  if (ed->num_refs) {
    return &ed->refs[ed->active];
  }
  return NULL;
}

static void ed_append_line(tcq_t* q, ed_in_t* in, size_t row, size_t column, size_t length) {
  char* a = alloca(length);
  ed_in_fill_line(in, a, row, column, length, ' ');
  append_output(q, a, length);
}

static void ed_redraw_everything(ed_view_t* ed) {
  if (!ed->num_refs) {
    ed_reset(ed);
    return;
  }
  line_process_input_changed(ed->line_view, REDRAW);
  size_t in_row, in_col;
  i_ref_t* ref = ed_get_active_ref(ed);
  ed_in_get_cursor_position(ref->in, &in_row, &in_col);
  tcq_t* q = alloc_command_queue(ed->area.rows * ed->area.columns * 2);
  append_move_cursor(q, ed->area.origin_x, ed->area.origin_y);
  append_options(q, FONT_DEFAULT, FG_BLACK, BG_WHITE);
  for (size_t i = 0; i < ed->area.rows; i++) {
    ed_append_line(q, ref->in, ref->file_x + i, ref->file_y, ed->area.columns);
    append_move_cursor(q, ed->area.origin_x + i + 1, ed->area.origin_y);
  }
  append_move_cursor(q, ed->area.origin_x + in_row - ref->file_x, ed->area.origin_y + in_col - ref->file_y);
  execute(q);
  free_command_queue(q);
}

static void ed_move_cursor(ed_view_t* ed) {
  size_t in_row, in_col;
  i_ref_t* ref = ed_get_active_ref(ed);
  tcq_t* q = alloc_command_queue(32);
  ed_in_get_cursor_position(ref->in, &in_row, &in_col);
  append_move_cursor(q, ed->area.origin_x + in_row - ref->file_x, ed->area.origin_y + in_col - ref->file_y);
  execute(q);
  free_command_queue(q);
}

ed_view_t* ed_init_editor(size_t origin_x, size_t origin_y, size_t rows, size_t columns) {
  ed_view_t* ed = malloc(sizeof(ed_view_t));
  ed->line_view = line_init_editor(ed, origin_x, origin_y, rows, 1);
  ed->area.origin_x = origin_x;
  ed->area.origin_y = origin_y + 1;
  ed->area.rows = rows;
  ed->area.columns = columns - 1;
  ed->active = 1;
  ed->refs = NULL;
  ed->num_refs = 0;
  ed_redraw_everything(ed);
  return ed;
}

void ed_free(ed_view_t* view) {
  if (view->num_refs) {
    free(view->refs);
  }
  line_free(view->line_view);
  free(view);
}

void ed_register_model(ed_view_t* view, ed_in_t* model) {
  for (size_t i = 0; i < view->num_refs; i++) {
    if (view->refs[i].in == model) {
      return;
    }
  }
  view->num_refs++;
  view->refs = realloc(view->refs, view->num_refs * sizeof(i_ref_t));
  view->refs[view->num_refs - 1].file_x = 0;
  view->refs[view->num_refs - 1].file_y = 0;
  view->refs[view->num_refs - 1].in = model;
}

void ed_unregister_active_model(ed_view_t* ed) {
  if (ed->num_refs == 0) {
    return;
  }
  for (size_t i = ed->active + 1; i < ed->num_refs; i++) {
    ed->refs[i - 1] = ed->refs[i];
  }
  ed->num_refs--;
  ed->refs = realloc(ed->refs, ed->num_refs * sizeof(i_ref_t));
  ed->active = ed->active ? ed->active - 1 : 0;
  ed_redraw_everything(ed);
}

void ed_set_model_active(ed_view_t* view, ed_in_t* model) {
  for (size_t i = 0; i < view->num_refs; i++) {
    if (view->refs[i].in != model) {
      continue;
    }
    if (i != view->active) {
      view->active = i;
      ed_redraw_everything(view);
    }
    return;
  }
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

static int ed_adapt_to_input_position(ed_view_t* ed) {
  i_ref_t* ref = ed_get_active_ref(ed);
  size_t in_row, in_col;
  int ret = 0;
  ed_in_get_cursor_position(ref->in, &in_row, &in_col);
  if (ref->file_x > in_row) {
    ref->file_x = in_row;
    ret = 1;
  } else if (ref->file_x + ed->area.rows <= in_row + 1) {
    ref->file_x = in_row - ed->area.rows + 1;
    ret = 1;
  }
  if (ref->file_y > in_col) {
    ref->file_y = in_col;
    ret = 1;
  } else if (ref->file_y + ed->area.columns < in_col + 1) {
    ref->file_y = in_col - ed->area.columns + 1;
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
    i_ref_t* ref = ed_get_active_ref(ed);
    ed_in_get_cursor_position(ref->in, &in_row, &in_col);
    tcq_t* q = alloc_command_queue(ed->area.columns + 64);
    append_move_cursor(q, ed->area.origin_x + in_row - ref->file_x, ed->area.origin_y);
    size_t affected_row = in_row;
    append_options(q, FONT_DEFAULT, FG_BLACK, BG_WHITE);
    ed_append_line(q, ref->in, affected_row, ref->file_y, ed->area.columns);
    append_move_cursor(q, ed->area.origin_x + in_row - ref->file_x, ed->area.origin_y + in_col - ref->file_y);
    execute(q);
    free_command_queue(q);
  }
}

void ed_process_input_changed(ed_view_t* ed, enum CALLBACK_TYPE type) {
  switch (type) {
  case EDITOR_INPUT_CURSOR: {
    ed_process_cursor_changed(ed);
    break;
  }
  case EDITOR_INPUT_LINE: {
    ed_process_line_changed(ed);
    break;
  }
  case REDRAW: {
    ed_adapt_to_input_position(ed);
    ed_redraw_everything(ed);
    break;
  }
  case EDITOR_TOGGLE_LINE_NUMBERING: {
    line_process_input_changed(ed->line_view, EDITOR_TOGGLE_LINE_NUMBERING);
    ed_redraw_everything(ed);
    break;
  }
  default:
    break;
  }
}

void ed_move_up_screen(ed_view_t* view) {
  i_ref_t* ref = ed_get_active_ref(view);
  size_t in_row, in_col;
  ed_in_get_cursor_position(ref->in, &in_row, &in_col);
  if (in_row - view->area.rows > 0) {
    ref->file_x = ref->file_x > view->area.rows ? ref->file_x > view->area.rows : 0;
  }
  ed_in_move_up_line(ref->in, view->area.rows);
  if (in_row - view->area.rows > 0) {
    ed_redraw_everything(view);
  }
}

void ed_move_down_screen(ed_view_t* view) {
  i_ref_t* ref = ed_get_active_ref(view);
  size_t in_row, in_col, in_num_lines;
  ed_in_get_cursor_position(ref->in, &in_row, &in_col);
  in_num_lines = ed_in_get_num_lines(ref->in);
  if (in_row + view->area.rows < in_num_lines - 1) {
    ref->file_x = ref->file_x + view->area.rows;
  }
  //Adjust for last page
  if (ref->file_x > in_num_lines - 1 - view->area.rows) {
    ref->file_x = in_num_lines - view->area.rows;
  }
  ed_in_move_down_line(ref->in, view->area.rows);
  if (in_row + view->area.rows < in_num_lines - 1) {
    ed_redraw_everything(view);
  }
}

void ed_resize(ed_view_t* view, size_t origin_x, size_t origin_y, size_t rows, size_t columns) {
  view->area.origin_x = origin_x;
  view->area.origin_y = origin_y;
  view->area.rows = rows;
  view->area.columns = columns;
}

area_t ed_get_screen_area(ed_view_t* view) {
  return view->area;
}

int ed_get_visible_file_area(ed_view_t* view, size_t* file_x, size_t* rows) {
  if (!view->num_refs) {
    return 1;
  }
  if (file_x) {
    (*file_x) = ed_get_active_ref(view)->file_x;
  }
  if (rows) {
    (*rows) = minu(ed_in_get_num_lines(ed_get_active_ref(view)->in) - ed_get_active_ref(view)->file_x, view->area.rows);
  }
  return 0;
}
