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

typedef struct editor_input_t {
  enum CALLBACK_TYPE type;
  line_t* lines;
  size_t num_lines;

  //line and column of the cursor on the data set!
  size_t row, column;

  void (*controller_call_back) (void* controller, enum CALLBACK_TYPE);
  void* controller;
} ed_in_t;

ed_in_t* init_editor_input(void (*controller_call_back) (void* controller, enum CALLBACK_TYPE callback_type), void* controller) {
  ed_in_t* in = malloc(sizeof(ed_in_t));
  in->row = 0;
  in->column = 0;
  in->lines = malloc(sizeof(line_t));
  in->num_lines = 1;
  in->lines[0].line = malloc(sizeof(char));
  in->lines[0].length = 1;
  in->lines[0].pos = 0;
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
  in->column = in->lines[in->row].pos;
}

static inline void move_forward_character(ed_in_t* in) {
  if(in->row == in->num_lines - 1 && in->column == in->lines[in->row].pos) {
    return;
  }
  if(in->column < in->lines[in->row].pos) {
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
      break;
    }
    case DOWN: {
      #pragma message "implement"
      break;
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

static void ed_input_printable_character(ed_in_t* in, key_t k) {
  line_t* line = &in->lines[in->row];
  if(line->length == line->pos) {
    line->length <<= 1;
    line->line = realloc(line->line, line->length);
  }
  for(size_t i = line->pos; i > in->column; i--) {
    line->line[i] = line->line[i - 1];
  }
  line->line[in->column] = k.key;
  line->pos++;
  in->column++;
  in->controller_call_back((c_t*)in->controller, EDITOR_INPUT_LINE);
}

static void ed_input_LF(ed_in_t* in) {
  in->num_lines++;
  in->lines = realloc(in->lines, in->num_lines * sizeof(line_t));
  line_t* old_line = &in->lines[in->row];
  line_t* new_line = &in->lines[in->row + 1];
  new_line->pos = old_line->pos - in->column;
  size_t np2 = next_pow_2(new_line->pos);
  new_line->length = !np2 ? 1 : np2;
  new_line->line = malloc(new_line->length);
  memcpy(new_line->line, old_line->line + in->column, new_line->pos);
  old_line->pos = in->column;
  if(old_line->pos == old_line->length) {
    old_line->length <<= 1;
    old_line->line = realloc(old_line->line, old_line->length);
  }
  old_line->line[old_line->pos] = '\n';
  in->row++;
  in->column = 0;
  in->controller_call_back((c_t*)in->controller, EDITOR_INPUT_ALL);
}

void ed_input(ed_in_t* in, key_t k) {
  if(IS_PRINTABLE(k)) {
    ed_input_printable_character(in, k);
    return;
  } else if (k.ascii) {
    if(k.key == 0x0D) {
      ed_input_LF(in);
    }
  } else if(!k.ascii) {
    ed_non_ascii_input(in, k);
    return;
  }
}

void fill_line(ed_in_t* in, char* line_to_fill, size_t row, size_t column, size_t length, char fill_character) {
  if(in->num_lines <= row) {
    memset(line_to_fill, fill_character, length);
    return;
  }
  if(in->lines[row].pos <= column) {
    memset(line_to_fill, fill_character, length);
    return;
  }
  #pragma message "replace for non-printable characters"
  size_t cpy_length = minu(in->lines[row].pos - column, length);
  memcpy(line_to_fill, in->lines[row].line + column, cpy_length);
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
