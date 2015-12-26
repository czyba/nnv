#ifndef _CONTROLLER_H
#define _CONTROLLER_H

#include "input.h"
#include "controller_callback.h"

typedef struct controller_t c_t;

c_t* c_init_controller();
void c_free_controller(c_t* controller);

void c_open_file(c_t* controller, char* filename);
void c_input_key(c_t* controller, key_t key);

#endif
