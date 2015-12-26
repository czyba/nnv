#ifndef _CONTROLLER_H
#define _CONTROLLER_H

#include "input.h"
#include "view.h"
#include "model.h"

typedef struct controller_t c_t;

c_t* init_controller();
void free_controller(c_t* controller);

void open_file(c_t* controller, char* filename);
void input_key(c_t* controller, key_t key);

#endif
