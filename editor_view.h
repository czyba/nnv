#ifndef _EDITOR_VIEW_H
#define _EDITOR_VIEW_H

#include "editor_model.h"
#include "screen.h"

typedef struct editor_view_t ed_view_t;
ed_view_t* ed_init_editor(ed_in_t* in, int origin_x, int origin_y, int rows, int columns);
void ed_free(ed_view_t* view);

void ed_reset(ed_view_t* view);
area_t ed_get_area(ed_view_t* view);
ed_in_t* ed_get_model(ed_view_t* view);

void ed_move_up_screen(ed_view_t* view);
void ed_move_down_screen(ed_view_t* view);
void ed_process_input_changed(ed_view_t* view, enum CALLBACK_TYPE type);

#endif
