#include "view.h"
#include "termout.h"
#include <stdlib.h>
#include <string.h>
#include <alloca.h>

typedef struct area_t {
  int origin_x, origin_y;
  int rows, columns;
  char** content;
} area_t;

struct editor_view_t {
  area_t area;
  //line and column of the cursor in the editor!
  int r_line, r_column;

  // Stores the bottom left position of the file
  int file_x, file_y;

  ed_in_t* in;
};

static void ed_append_line(tcq_t* q, ed_in_t* in, size_t row, size_t column, size_t length) {
  char* a = alloca(length);
  fill_line(in, a, row, column, length, ' ');
  append_output(q, a, length);
}

static void ed_redraw_everything(ed_view_t* ed) {
  //TODO: handle cursor properly
  ed_in_t* in = ed->in;
  tcq_t* q = alloc_command_queue(ed->area.rows * ed->area.columns * 2);
  append_options(q, FONT_DEFAULT, FG_BLACK, BG_WHITE);
  for(int i = 0; i < ed->area.rows; i++) {
    ed_append_line(q, in, ed->file_x, ed->file_y, ed->area.columns);
    append_move_cursor(q, ed->area.origin_x + i + 1, ed->area.origin_y);
  }
  append_move_cursor(q, ed->area.origin_x + ed->r_line, ed->area.origin_y + ed->r_column);
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
  ed->area.content = malloc(sizeof(char*) * ed->area.rows);
  for(int i = 0; i < ed->area.rows; i++) {
    ed->area.content[i] = malloc(sizeof(char) * ed->area.columns);
    memset(ed->area.content[i], ' ', ed->area.columns);
  }
  ed->r_line = 0;
  ed->r_column = 0;
  ed->file_x = 0;
  ed->file_y = 0;
  ed_redraw_everything(ed);
  return ed;
}

void ed_process_input_changed(ed_view_t* ed){
  ed_in_t* in = ed->in;
  (void) in;
}
