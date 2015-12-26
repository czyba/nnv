#ifndef _TAB_MODEL_H
#define _TAB_MODEL_H

#include "controller_callback.h"
#include "editor_model.h"
#include <stddef.h>

typedef struct tab_model_t tab_in_t;

tab_in_t* init_tab_input(controller_call_back_t cb, void* controller);
void tab_in_free(tab_in_t* in);

int tab_in_register_tab(tab_in_t* in, char* relative_file_path);
void tab_in_unregister_tab(tab_in_t* in);

void tab_next(tab_in_t* in);
void tab_previous(tab_in_t* in);

ed_in_t** tab_get_tabs(tab_in_t* in);
size_t tab_get_index(tab_in_t* in);
size_t tab_get_num_names(tab_in_t* in);

ed_in_t* tab_get_active_tab(tab_in_t* in);

#endif
