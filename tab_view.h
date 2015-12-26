#ifndef _TAB_VIEW_H
#define _TAB_VIEW_H

#include "tab_model.h"

typedef struct tab_view_t tab_view_t;

tab_view_t* tab_init_editor(tab_in_t* in, int origin_x, int origin_y, int rows, int columns);
void tab_free(tab_view_t* view);

tab_in_t* tab_get_model(tab_view_t* view);

void tab_process_input_changed(tab_view_t* view, enum CALLBACK_TYPE type);

#endif
