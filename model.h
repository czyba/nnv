#ifndef _MODEL_H
#define _MODEL_H

#include "input.h"

enum CALLBACK_TYPE {
  EDITOR_INPUT_ALL,
  EDITOR_INPUT_LINE,
  EDITOR_INPUT_CURSOR
};

typedef struct editor_input_t ed_in_t;

ed_in_t* init_editor_input(void (*controller_call_back)(void* controller, enum CALLBACK_TYPE callback_type), void* controller);
void ed_in_free(ed_in_t* in);

void ed_in_load_file(ed_in_t* in, char* filename);
void ed_in_save_file(ed_in_t* in);

void ed_in_fill_line(ed_in_t* in, char* line_to_fill, size_t row, size_t column, size_t lenth, char fill_character);
void ed_in_get_cursor_position(ed_in_t* in, size_t* row, size_t* column);

int ed_in_at_origin(ed_in_t* in);
size_t ed_in_get_num_lines(ed_in_t* in);

void ed_in_move_up_line(ed_in_t* in, size_t lines);
void ed_in_move_down_line(ed_in_t* in, size_t lines);
void ed_in_move_back_character(ed_in_t* in);
void ed_in_move_forward_character(ed_in_t* in);
void ed_in_move_home(ed_in_t* in);
void ed_in_move_end(ed_in_t* in);
void ed_in_delete_at_cursor(ed_in_t* in);
void ed_in_input_printable_character(ed_in_t* in, key_t k);
void ed_in_input_LF(ed_in_t* in);

#endif
