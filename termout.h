#include <stddef.h>
#include "fontoptions.h"

struct terminal_command_queue_t;

typedef struct terminal_commands_queue_t tcq_t;

tcq_t* get_command_queue(size_t size);

int move_cursor(tcq_t* queue, int x, int y);

//TODO: enum!
int append_options(tcq_t* queue, enum FONT_OPTION font_options, enum FOREGROUND_COLORS foreground_color, enum BACKGROUND_COLORS background_color);
int append_output(tcq_t* queue, char* output);
int execute(tcq_t* queue);


// *_r options result in setting the cursor back to it's current position
int append_options_r(tcq_t* queue, enum FONT_OPTION font_options, enum FOREGROUND_COLORS foreground_color, enum BACKGROUND_COLORS background_color);
int append_output_r(tcq_t* queue, char* output);
int execute_r(tcq_t* queue);
