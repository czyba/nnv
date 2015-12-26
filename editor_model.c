#include "editor_model.h"
#include "input.h"
#include "controller.h"
#include "basic_math.h"
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <unistd.h>
#include <sys/types.h>

#include <stdio.h>

typedef struct line_t {
  char* line;
  size_t length;
  size_t pos;
} line_t;

typedef struct editor_input_t {
  line_t* lines;
  size_t num_lines;

  //line and column of the cursor on the data set!
  size_t row, column;

  char* file_name;
  int fd;

  cb_t cb;
} ed_in_t;

static int ed_in_read_line(line_t* line, char* buf, size_t length) {
  size_t i = 0;
  for (; i < length; i++) {
    if (buf[i] == ASCII_LF) {
      break;
    }
  }
  size_t new_length;
  size_t new_pos = line->pos + i;
  new_length = next_pow_2(new_pos);
  if (new_length > line->length) {
    line->line = realloc(line->line, new_length);
  }
  memcpy(line->line + line->pos, buf, i);
  line->length = new_length;
  line->pos = new_pos;
  if (buf[i] == ASCII_LF) {
    i++;
    i = -i;
  }
  return i;
}

void ed_in_load_file(ed_in_t* in, char* filename) {
  in->fd = open(filename, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
  if (in->fd < 0) {
    return;
  }
  char buf[4096];
  int chars_read = read(in->fd, buf, sizeof(buf));
  line_t* line = &in->lines[0];
  while (chars_read >= 0) {
    if (chars_read == 0) {
      break;
    }
    int i = 0;
    while (i < chars_read) {
      int tmp = ed_in_read_line(line, buf + i, chars_read - i);
      if (tmp < 0) {
        in->num_lines++;
        in->lines = realloc(in->lines, in->num_lines * sizeof(line_t));
        line = &in->lines[in->num_lines - 1];
        line->pos = 0;
        line->length = 1;
        line->line = malloc(sizeof(char));
        tmp = -tmp;
      }
      i += tmp;
    }
    chars_read = read(in->fd, buf, sizeof(buf));
  }
}

void ed_in_save_file(ed_in_t* in) {
  if (in->fd < 0) {
    return;
  }
  lseek(in->fd, 0, SEEK_SET);
  ftruncate(in->fd, 0);
  for (size_t i = 0; i < in->num_lines; i++) {
    write(in->fd, in->lines[i].line, in->lines[i].pos);
    if (i != in->num_lines - 1) {
      char c = ASCII_LF;
      write(in->fd, &c, 1);
    }
  }
}

ed_in_t* init_editor_input( controller_call_back_t cb, c_t* controller) {
  ed_in_t* in = malloc(sizeof(ed_in_t));
  in->row = 0;
  in->column = 0;
  in->lines = malloc(sizeof(line_t));
  in->num_lines = 1;
  in->lines[0].line = malloc(sizeof(char));
  in->lines[0].length = 1;
  in->lines[0].pos = 0;
  in->fd = -1;
  in->file_name = NULL;
  in->cb.cb = cb;
  in->cb.controller = controller;
  return in;
}

void ed_in_free(ed_in_t* in) {
  for (size_t i = 0; i < in->num_lines; i++) {
    free(in->lines[i].line);
  }
  free(in->lines);
  if (in->fd >= 0) {
    close(in->fd);
  }
  free(in);
}

void ed_in_move_up_line(ed_in_t* in, size_t lines) {
  in->row = in->row > lines ? in->row - lines : 0;
  if (in->column > in->lines[in->row].pos) {
    in->column = in->lines[in->row].pos;
  }
  cb_do_callback(&in->cb, EDITOR_INPUT_CURSOR);
}

void ed_in_move_down_line(ed_in_t* in, size_t lines) {
  in->row = in->row + lines < in->num_lines - 1 ? in->row + lines : in->num_lines - 1;
  if (in->column > in->lines[in->row].pos) {
    in->column = in->lines[in->row].pos;
  }
  cb_do_callback(&in->cb, EDITOR_INPUT_CURSOR);
}

void ed_in_move_back_character(ed_in_t* in) {
  if (in->row == 0 && in->column == 0) {
    return;
  }
  if (in->column > 0) {
    in->column--;
  } else {
    in->row--;
    in->column = in->lines[in->row].pos;
  }
  cb_do_callback(&in->cb, EDITOR_INPUT_CURSOR);
}

void ed_in_move_forward_character(ed_in_t* in) {
  if (in->row == in->num_lines - 1 && in->column == in->lines[in->row].pos) {
    return;
  }
  if (in->column < in->lines[in->row].pos) {
    in->column++;
  } else {
    in->row++;
    in->column = 0;
  }
  cb_do_callback(&in->cb, EDITOR_INPUT_CURSOR);
}

void ed_in_move_home(ed_in_t* in) {
  in->column = 0;
  cb_do_callback(&in->cb, EDITOR_INPUT_CURSOR);
}

void ed_in_move_end(ed_in_t* in) {
  in->column = in->lines[in->row].pos;
  in->cb.cb((c_t*) in->cb.controller, EDITOR_INPUT_CURSOR);
}

void ed_in_delete_at_cursor(ed_in_t* in) {
  if ((in->num_lines == 1 && in->lines[0].pos == 0) ||                          //empty, nothing to delete
      (in->row + 1 == in->num_lines && in->column == in->lines[in->row].pos)) { //or last character of file
    return;
  }
  if (in->column != in->lines[in->row].pos) {
    //just delete that character and move everything back!
    for (size_t i = in->column; i + 1 < in->lines[in->row].pos; i++) {
      in->lines[in->row].line[i] = in->lines[in->row].line[i + 1];
    }
    in->lines[in->row].pos--;
    //Make line buffer smaller if necessary
    if ((in->lines[in->row].pos > 0 && (in->lines[in->row].pos << 1) <= in->lines[in->row].length && in->row + 1 == in->num_lines) ||       //Last line
        (in->lines[in->row].pos > 0 && ((in->lines[in->row].pos + 1) << 1) <= in->lines[in->row].length && in->row + 1 != in->num_lines)) { //other lines
      in->lines[in->row].length >>= 1;
      in->lines[in->row].line = realloc(in->lines[in->row].line, in->lines[in->row].length);
    }
    in->cb.cb((c_t*) in->cb.controller, EDITOR_INPUT_LINE);
    return;
  }
  line_t* c_line = &in->lines[in->row];
  line_t* d_line = &in->lines[in->row + 1];
  size_t new_pos = c_line->pos + d_line->pos;
  size_t new_length;
  //1 merge line
  new_length = next_pow_2(new_pos);
  new_length = new_length ? new_length : 1;
  if (new_length > c_line->length) {
    c_line->line = realloc(c_line->line, new_length);
  }
  c_line->length = new_length;
  memcpy(c_line->line + c_line->pos, d_line->line, d_line->pos);
  c_line->pos = new_pos;
  //copy old lines
  free(d_line->line);
  for (size_t i = in->row + 1; i + 1 < in->num_lines; i++) {
    in->lines[i] = in->lines[i + 1];
  }
  //resize lines
  in->num_lines--;
  in->lines = realloc(in->lines, sizeof(line_t) * in->num_lines);
  in->cb.cb((c_t*) in->cb.controller, EDITOR_INPUT_ALL);
}

void ed_in_input_printable_character(ed_in_t* in, key_t k) {
  line_t* line = &in->lines[in->row];
  if (line->length == line->pos) {
    line->length <<= 1;
    line->line = realloc(line->line, line->length);
  }
  for (size_t i = line->pos; i > in->column; i--) {
    line->line[i] = line->line[i - 1];
  }
  line->line[in->column] = k.key;
  line->pos++;
  in->column++;
  in->cb.cb((c_t*) in->cb.controller, EDITOR_INPUT_LINE);
}

void ed_in_input_LF(ed_in_t* in) {
  in->num_lines++;
  in->lines = realloc(in->lines, in->num_lines * sizeof(line_t));
  for (size_t i = in->num_lines - 1; in->row < i - 1; i--) {
    in->lines[i] = in->lines[i - 1];
  }
  line_t* old_line = &in->lines[in->row];
  line_t* new_line = &in->lines[in->row + 1];
  new_line->pos = old_line->pos - in->column;
  size_t np2 = next_pow_2(new_line->pos);
  new_line->length = !np2 ? 1 : np2;
  new_line->line = malloc(new_line->length);
  memcpy(new_line->line, old_line->line + in->column, new_line->pos);
  old_line->pos = in->column;
  in->row++;
  in->column = 0;
  in->cb.cb((c_t*) in->cb.controller, EDITOR_INPUT_ALL);
}

int ed_in_at_origin(ed_in_t* in) {
  return (in->row == 0 && in->column == 0);
}

void ed_in_fill_line(ed_in_t* in, char* line_to_fill, size_t row, size_t column, size_t length, char fill_character) {
  if (in->num_lines <= row) {
    memset(line_to_fill, fill_character, length);
    return;
  }
  if (in->lines[row].pos <= column) {
    memset(line_to_fill, fill_character, length);
    return;
  }
#pragma message "replace for non-printable characters"
  size_t cpy_length = minu(in->lines[row].pos - column, length);
  memcpy(line_to_fill, in->lines[row].line + column, cpy_length);
  if (cpy_length < length) {
    for (size_t i = cpy_length; i < length; i++) {
      line_to_fill[i] = fill_character;
    }
  }
  return;
}

void ed_in_get_cursor_position(ed_in_t* in, size_t* row, size_t* column) {
  *row = in->row;
  *column = in->column;
}

size_t ed_in_get_num_lines(ed_in_t* in) {
  return in->num_lines;
}
