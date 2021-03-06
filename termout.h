#ifndef _TERMOUT_H
#define _TERMOUT_H

#include <stddef.h>
#include "fontoptions.h"

typedef struct terminal_command_queue_t tcq_t;

tcq_t* alloc_command_queue(size_t size);
int free_command_queue(tcq_t* queue);

int reset_queue(tcq_t* queue);

int get_cursor_position(size_t* line, size_t* column);
int append_move_cursor(tcq_t* queue, int x, int y);

//TODO: enum!
int append_options(tcq_t* queue, enum FONT_OPTION font_options, enum FOREGROUND_COLORS foreground_color, enum BACKGROUND_COLORS background_color);
int append_output(tcq_t* queue, char* output, size_t n);
int execute(tcq_t* queue);

int print_queue(tcq_t* queue);

// *_r options result in setting the cursor back to it's current position
//int append_output_r(tcq_t* queue, char* output, size_t n);
//int execute_r(tcq_t* queue);

#endif
