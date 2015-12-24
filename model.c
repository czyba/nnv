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
  enum MODEL_TYPE type;
  lines_t lines;
  //line and column of the cursor on the data set!
  size_t row, column;

  void (*controller_call_back) (void* controller, enum MODEL_TYPE);
  void* controller;
} ed_in_t;

ed_in_t* init_editor_input(void (*controller_call_back) (void* controller, enum MODEL_TYPE callback_type), void* controller) {
  ed_in_t* in = malloc(sizeof(ed_in_t));
  in->row = 0;
  in->column = 0;
  in->lines.lines = malloc(sizeof(line_t*));
  in->lines.num_lines = 1;
  in->lines.lines[0].line = malloc(sizeof(char));
  in->lines.lines[0].length = 1;
  in->lines.lines[0].pos = 0;
  in->controller_call_back = controller_call_back;
  in->controller = controller;
  return in;
}

void ed_input(ed_in_t* in, key_t k) {
  if(IS_PRINTABLE(k)) {
    line_t* line = &in->lines.lines[in->row];
    if(line->length == line->pos) {
      line->length <<= 1;
      line->line = realloc(line->line, line->length);
    }
    line->line[line->pos++] = k.key;
    in->column++;
    in->controller_call_back((c_t*)in->controller, MODEL_EDITOR_INPUT);
    return;
  }
}

void fill_line(ed_in_t* in, char* line_to_fill, size_t row, size_t column, size_t length, char fill_character) {
  if(in->lines.num_lines < row) {
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
