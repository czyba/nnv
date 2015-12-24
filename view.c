#include "view.h"
#include <stdlib.h>

typedef struct area_t {
  int origin_x, origin_y;
  int rows, columns;
  char** content;
} area_t;

struct editor_view_t {
  area_t area;
  //line and column of the cursor in the editor!
  int r_line, r_column;
  //
  int file_x, file_y;

  ed_in_t* in;
};

ed_view_t* ed_init_editor(ed_in_t* in, int origin_x, int origin_y, int rows, int columns) {
  ed_view_t* ed = malloc(sizeof(ed_view_t));
  ed->area.origin_x = origin_x;
  ed->area.origin_y = origin_y;
  ed->area.rows = rows;
  ed->area.columns = columns;
  ed->r_line = 0;
  ed->r_column = 0;
  ed->file_x = 0;
  ed->file_y = 0;
  ed->in = in;
  return ed;
}

void ed_process_input_changed(ed_view_t* view){
  ed_in_t* in = view->in;
}
