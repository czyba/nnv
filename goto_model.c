#include "goto_model.h"
#include "basic_math.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct goto_line_model_t {
  char* input;
  size_t pos;
  size_t length;
  size_t colon_pos;
  cb_t cb;
};

goto_in_t* init_goto_in(controller_call_back_t cb, c_t* controller) {
  goto_in_t* in = malloc(sizeof(goto_in_t));
  in->length = 0;
  in->colon_pos = 0;
  in->pos = 0;
  in->input = NULL;
  in->cb.cb = cb;
  in->cb.controller = controller;
  return in;
}
void free_goto_in(goto_in_t* model) {
  if (model->length) {
    free(model->input);
  }
  free(model);
}

size_t goto_in_get_pos(goto_in_t* model) {
  return model->pos;
}

void goto_in_fill_line(goto_in_t* model, char* buf, size_t start, size_t length, char fill_character) {
  size_t written = minu(length, model->length - start);
  memcpy(buf, model->input + start, written);
  if (written < length) {
    memset(buf + written, fill_character, length - written);
  }
}

void goto_in_reset(goto_in_t* in) {
  if (in->length) {
    in->length = 0;
    in->colon_pos = 0;
    in->pos = 0;
    free(in->input);
    in->input = NULL;
  }
  in->cb.cb(in->cb.controller, GOTO_LINE_CHANGED);
}

static void goto_in_input_character(goto_in_t* in, char c) {
  in->length++;
  in->input = realloc(in->input, in->length);
  for (size_t i = in->pos + 1; i < in->length; i++) {
    in->input[i] = in->input[i - 1];
  }
  in->input[in->pos] = c;
  in->pos++;
}

static int goto_in_move_cursor(goto_in_t* in, int steps) {
  if (in->pos >= in->length && steps >= 0) {
    return 0;
  }
  if (in->pos == 0 && steps <= 0) {
    return 0;
  }
  if (in->pos + steps > in->length) {
    in->pos = in->length;
    return 1;
  }
  if (steps < 0 && (size_t) abs(steps) > in->pos) {
    in->pos = 0;
    return 1;
  }
  in->pos += steps;
  return 1;
}

static int goto_in_delete_character(goto_in_t* in) {
  if (in->pos == in->length) {
    return 0;
  }
  if (in->input[in->pos] == ':') {
    in->colon_pos = 0;
  }
  for (size_t i = in->pos + 1; i < in->length; i++) {
    in->input[i - 1] = in->input[i];
  }
  in->length--;
  in->input = realloc(in->input, in->length);
  return 1;
}

void goto_in_get_line_column(goto_in_t* model, size_t* line, size_t* column) {
  if (model->colon_pos) {
    if (model->colon_pos != model->length) {
      sscanf(model->input, "%lu:%lu", line, column);
      return;
    }
    *column = 0;
    sscanf(model->input, "%lu:", line);
    return;
  }
  *column = 0;
  sscanf(model->input, "%lu", line);
  return;
}

void goto_in_input_key(goto_in_t* model, key_t key) {
  if (key.ascii) {
    if (IS_DIGIT(key)) {
      goto_in_input_character(model, key.key);
      model->cb.cb(model->cb.controller, GOTO_LINE_CHANGED);
    } else if (key.key == ':' && model->pos != 0 && model->colon_pos == 0) {
      model->colon_pos = model->pos;
      goto_in_input_character(model, key.key);
      model->cb.cb(model->cb.controller, GOTO_LINE_CHANGED);
    } else if (key.key == ASCII_DEL && model->pos != 0) {
      if (goto_in_move_cursor(model, -1)) {
        goto_in_delete_character(model);
        model->cb.cb(model->cb.controller, GOTO_LINE_CHANGED);
      }
    } else if (key.key == ASCII_ESC) {
      model->cb.cb(model->cb.controller, GOTO_LINE_CLOSE);
    } else if (key.key == ASCII_CR && model->length) {
      model->cb.cb(model->cb.controller, GOTO_LINE_EXECUTE);
    }
    return;
  }
  switch (key.nkey & KEY_MASK) {
  case KEY_LEFT: {
    if (goto_in_move_cursor(model, -1)) {
      model->cb.cb(model->cb.controller, GOTO_LINE_CHANGED);
    }
    break;
  }
  case KEY_RIGHT: {
    if (goto_in_move_cursor(model, 1)) {
      model->cb.cb(model->cb.controller, GOTO_LINE_CHANGED);
    }
    break;
  }
  case KEY_HOME: {
    if (goto_in_move_cursor(model, -((int) model->length))) {
      model->cb.cb(model->cb.controller, GOTO_LINE_CHANGED);
    }
    break;
  }
  case KEY_END: {
    if (goto_in_move_cursor(model, model->length)) {
      model->cb.cb(model->cb.controller, GOTO_LINE_CHANGED);
    }
    break;
  }
  case KEY_DEL: {
    if (goto_in_delete_character(model)) {
      model->cb.cb(model->cb.controller, GOTO_LINE_CHANGED);
    }
    break;
  }
  default:
    break;
  }
}
