#ifndef _MODEL_H
#define _MODEL_H

#include "input.h"

enum MODEL_TYPE {
  MODEL_EDITOR_INPUT
};

typedef struct editor_input_t ed_in_t;

ed_in_t* init_editor_input(void (*controller_call_back) (void* controller, enum MODEL_TYPE callback_type), void* controller);
void ed_input(ed_in_t* in, key_t k);

#endif
