#include "model.h"
#include "input.h"
#include "controller.h"
#include "basic_math.h"
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

typedef struct line_t {
  char* line;
  size_t length;
  size_t pos;
} line_t;

typedef struct lines_t {
  line_t* lines;
  size_t num_lines;
} lines_t;

typedef struct editor_input_t {
  enum CALLBACK_TYPE type;
  lines_t lines;
  //line and column of the cursor on the data set!
  size_t row, column;

  void (*controller_call_back) (void* controller, enum CALLBACK_TYPE);
  void* controller;
} ed_in_t;

ed_in_t* init_editor_input(void (*controller_call_back) (void* controller, enum CALLBACK_TYPE callback_type), void* controller) {
  ed_in_t* in = malloc(sizeof(ed_in_t));
  in->row = 0;
  in->column = 0;
  in->lines.lines = malloc(sizeof(line_t));
  in->lines.num_lines = 1;
  in->lines.lines[0].line = malloc(sizeof(char));
  in->lines.lines[0].length = 1;
  in->lines.lines[0].pos = 0;
  in->controller_call_back = controller_call_back;
  in->controller = controller;
  return in;
}

static inline void move_back_character(ed_in_t* in) {
  if(in->row == 0 && in->column == 0) {
    return;
  }
  if(in->column > 0) {
    in->column--;
    return;
  }
  in->row--;
  in->column = in->lines.lines[in->row].pos;
}

static inline void move_forward_character(ed_in_t* in) {
  if(in->row == in->lines.num_lines - 1 && in->column == in->lines.lines[in->row].pos) {
    return;
  }
  if(in->column < in->lines.lines[in->row].pos) {
    in->column++;
    return;
  }
  in->row++;
  in->column = 0;
}

void ed_non_ascii_input(ed_in_t* in, key_t k) {
  switch(k.nkey & KEY_MASK) {
    case UP: {
      #pragma message "implement"
    }
    case DOWN: {
      #pragma message "implement"
    }
    case RIGHT: {
      move_forward_character(in);
      in->controller_call_back((c_t*)in->controller, EDITOR_INPUT_CURSOR);
      break;
    }
    case LEFT: {
      move_back_character(in);
      in->controller_call_back((c_t*)in->controller, EDITOR_INPUT_CURSOR);
      break;
    }
  }
}

void ed_input(ed_in_t* in, key_t k) {
  if(IS_PRINTABLE(k)) {
    line_t* line = &in->lines.lines[in->row];
    if(line->length == line->pos) {
      line->length <<= 1;
      line->line = realloc(line->line, line->length);
    }
    #pragma message "Fix this shit, use row / column not pos"
    line->line[line->pos++] = k.key;
    in->column++;
    in->controller_call_back((c_t*)in->controller, EDITOR_INPUT_LINE);
    return;
  }
  if(!k.ascii) {
    ed_non_ascii_input(in, k);
    return;
  }
}

void fill_line(ed_in_t* in, char* line_to_fill, size_t row, size_t column, size_t length, char fill_character) {
  if(in->lines.num_lines <= row) {
    memset(line_to_fill, fill_character, length);
    return;
  }
  if(in->lines.lines[row].pos <= column) {
    memset(line_to_fill, fill_character, length);
    return;
  }
  #pragma message "replace for non-printable characters"
  size_t cpy_length = minu(in->lines.lines[row].pos - column, length);
  memcpy(line_to_fill, in->lines.lines[row].line + column, cpy_length);
  if(cpy_length < length) {
    for(size_t i = cpy_length; i < length; i++) {
      line_to_fill[i] = fill_character;
    }
  }
  return;
}

void ed_in_get_cursor_position(ed_in_t* in, size_t* row, size_t* column) {
  *row = in->row;
  *column = in->column;
}
