#ifndef _VIEW_H
#define _VIEW_H

#include "model.h"

typedef struct editor_view_t ed_view_t;
ed_view_t* ed_init_editor(ed_in_t* in, int origin_x, int origin_y, int rows, int columns);

ed_in_t* ed_get_model(ed_view_t* view);

void ed_process_input_changed(ed_view_t* view, enum CALLBACK_TYPE type);

#endif
