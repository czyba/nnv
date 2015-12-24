#ifndef _MODEL_H
#define _MODEL_H

#include "input.h"

enum CALLBACK_TYPE {
  EDITOR_INPUT_ALL,
  EDITOR_INPUT_LINE,
  EDITOR_INPUT_CURSOR
};

typedef struct editor_input_t ed_in_t;

ed_in_t* init_editor_input(void (*controller_call_back) (void* controller, enum CALLBACK_TYPE callback_type), void* controller);
void ed_input(ed_in_t* in, key_t k);

void fill_line(ed_in_t* in, char* line_to_fill, size_t row, size_t column, size_t lenth, char fill_character);
void ed_in_get_cursor_position(ed_in_t* in, size_t* row, size_t* column);

#endif
