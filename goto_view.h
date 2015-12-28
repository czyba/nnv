#ifndef _GOTO_VIEW_H
#define _GOTO_VIEW_H

#include <stddef.h>
#include "controller_callback.h"
#include "goto_model.h"

typedef struct goto_view_t goto_view_t;

goto_view_t* goto_init_view(goto_in_t* in, size_t origin_x, size_t origin_y, size_t columns);
void goto_free(goto_view_t* view);

void goto_process_input_changed(goto_view_t* view, enum CALLBACK_TYPE type);

#endif
