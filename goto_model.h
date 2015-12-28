#ifndef _GOTO_MODEL_H
#define _GOTO_MODEL_H

#include "controller_callback.h"
#include "controller.h"
#include "input.h"

typedef struct goto_line_model_t goto_in_t;

goto_in_t* init_goto_in(controller_call_back_t cb, c_t* controller);
void free_goto_in(goto_in_t* model);

void goto_in_input_key(goto_in_t* model, key_t key);

#endif
