#ifndef _LINE_VIEW_H
#define _LINE_VIEW_H

#include "controller_callback.h"
#include "editor_view.h"
#include "screen.h"

typedef struct line_view_t line_view_t;
line_view_t* line_init_editor(ed_view_t* editor, size_t origin_x, size_t origin_y, size_t rows, size_t columns);
void line_free(line_view_t* view);

void line_process_input_changed(line_view_t* view, enum CALLBACK_TYPE type);

void line_resize(line_view_t* view, size_t origin_x, size_t origin_y, size_t rows, size_t columns);
#endif
