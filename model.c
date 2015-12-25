#include "model.h"
#include "input.h"
#include "controller.h"
#include "basic_math.h"
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#include <unistd.h>
#include <stdio.h>

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

static inline void move_up_line(ed_in_t* in) {
  if(in->row == 0) {
    return;
  }
  in->row--;
  if(in->column > in->lines[in->row].pos) {
    in->column = in->lines[in->row].pos;
  }
  in->controller_call_back((c_t*)in->controller, EDITOR_INPUT_CURSOR);
}

static inline void move_down_line(ed_in_t* in) {
  if(in->row == in->num_lines - 1) {
    return;
  }
  in->row++;
  if(in->column > in->lines[in->row].pos) {
    in->column = in->lines[in->row].pos;
  }
  in->controller_call_back((c_t*)in->controller, EDITOR_INPUT_CURSOR);
}

static inline void move_back_character(ed_in_t* in) {
  if(in->row == 0 && in->column == 0) {
    return;
  }
  if(in->column > 0) {
    in->column--;
  } else {
    in->row--;
    in->column = in->lines[in->row].pos;
  }
  in->controller_call_back((c_t*)in->controller, EDITOR_INPUT_CURSOR);
}

static inline void move_forward_character(ed_in_t* in) {
  if(in->row == in->num_lines - 1 && in->column == in->lines[in->row].pos) {
    return;
  }
  if(in->column < in->lines[in->row].pos) {
    in->column++;
  } else {
    in->row++;
    in->column = 0;
  }
  in->controller_call_back((c_t*)in->controller, EDITOR_INPUT_CURSOR);
}

static inline void delete_at_cursor(ed_in_t* in) {
  if((in->num_lines == 1 && in->lines[0].pos == 0) || //empty, nothing to delete
     (in->row + 1 == in->num_lines && in->column == in->lines[in->row].pos)) { //or last character of file
    return;
  }
  if(in->column != in->lines[in->row].pos) {
    //just delete that character and move everything back!
    for(size_t i = in->column; i + 1 < in->lines[in->row].pos; i++) {
      in->lines[in->row].line[i] = in->lines[in->row].line[i + 1];
    }
    //Copy \n if it exists...
    if(in->row + 1 != in->num_lines) {
      in->lines[in->row].line[in->lines[in->row].pos - 1] = in->lines[in->row].line[in->lines[in->row].pos];
    }
    in->lines[in->row].pos--;
    //Make line buffer smaller if necessary
    if((in->lines[in->row].pos > 0 && (in->lines[in->row].pos << 1) <= in->lines[in->row].length && in->row + 1 == in->num_lines) || //Last line
      (in->lines[in->row].pos > 0 && ((in->lines[in->row].pos + 1) << 1) <= in->lines[in->row].length && in->row + 1 != in->num_lines)) { //other lines
      in->lines[in->row].length >>= 1;
      in->lines[in->row].line = realloc(in->lines[in->row].line, in->lines[in->row].length);
    }
    in->controller_call_back((c_t*)in->controller, EDITOR_INPUT_LINE);
    return;
  }
  line_t* c_line = &in->lines[in->row];
  line_t* d_line = &in->lines[in->row + 1];
  size_t new_pos = c_line->pos + d_line->pos;
  size_t new_length;
  //1 merge line
  if(in->row + 2 == in->num_lines) {
    //Last line is somewhat special
    new_length = next_pow_2(new_pos);
  } else {
    new_length = next_pow_2(new_pos + 1);
  }
  if(new_length > c_line->length) {
    c_line->line = realloc(c_line->line, new_length);
  }
  c_line->length = new_length;
  memcpy(c_line->line + c_line->pos, d_line->line, d_line->pos);
  c_line->pos = new_pos;
  if(in->row + 2 != in->num_lines) {
    //Copy the \n
    c_line->line[c_line->pos] = '\n';
  }
  //copy old lines
  free(d_line->line);
  for(size_t i = in->row + 1; i + 1 < in->num_lines; i++) {
    in->lines[i] = in->lines[i+1];
  }
  //resize lines
  in->num_lines--;
  in->lines = realloc(in->lines, sizeof(line_t) * in->num_lines);
  in->controller_call_back((c_t*)in->controller, EDITOR_INPUT_ALL);
}

void ed_non_ascii_input(ed_in_t* in, key_t k) {
  switch(k.nkey & KEY_MASK) {
    case UP: {
      move_up_line(in);
      break;
    }
    case DOWN: {
      move_down_line(in);
      break;
    }
    case RIGHT: {
      move_forward_character(in);
      break;
    }
    case LEFT: {
      move_back_character(in);
      break;
    }
    case DEL: {
      delete_at_cursor(in);
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
  for(size_t i = in->num_lines - 1; in->row < i - 1; i--) {
    in->lines[i] = in->lines[i - 1];
  }
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
  if(k.ascii) {
    if(IS_PRINTABLE(k)) {
      ed_input_printable_character(in, k);
      return;
    }
    switch(k.key) {
      case 0x0D: {
        ed_input_LF(in);
        break;
      }
      case ASCII_DEL: {
        if(in->row != 0 || in->column != 0) {
          move_back_character(in);
          delete_at_cursor(in);
        }
        break;
      }
    }
    return;
  }
  ed_non_ascii_input(in, k);
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
