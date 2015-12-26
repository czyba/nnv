#ifndef _EDITOR_VIEW_H
#define _EDITOR_VIEW_H

#include "controller_callback.h"
#include "editor_model.h"
#include "screen.h"

typedef struct editor_view_t ed_view_t;
ed_view_t* ed_init_editor(int origin_x, int origin_y, int rows, int columns);
void ed_free(ed_view_t* view);

void ed_reset(ed_view_t* view);

void ed_register_model(ed_view_t* view, ed_in_t* model);
void ed_set_model_active(ed_view_t* view, ed_in_t* model);

void ed_move_up_screen(ed_view_t* view);
void ed_move_down_screen(ed_view_t* view);
void ed_process_input_changed(ed_view_t* view, enum CALLBACK_TYPE type);

#endif
