#include "tab_model.h"
#include "stdlib.h"
#include "string.h"

struct tab_model_t {
  char** tab_names;
  size_t num_tabs;
  size_t active_tab;
  cb_t cb;
};

tab_in_t* init_tab_input(controller_call_back_t cb, void* controller) {
  tab_in_t* in = malloc(sizeof(tab_in_t));
  in->num_tabs = 0;
  in->active_tab = 0;
  in->tab_names = NULL;
  in->cb.cb = cb;
  in->cb.controller = controller;
  return in;
}

void tab_in_free(tab_in_t* in) {
  for (size_t i = 0; i < in->num_tabs; i++) {
    free(in->tab_names[i]);
  }
  free(in);
}

void tab_in_register_tab(tab_in_t* in, char* relative_file_path) {
#pragma message "check whether tab exists"
  in->num_tabs++;
  in->active_tab = in->num_tabs - 1;
  in->tab_names = realloc(in->tab_names, in->num_tabs * sizeof(char*));
  char* name = malloc(strlen(relative_file_path) + 1);
  strcpy(name, relative_file_path);
  in->tab_names[in->active_tab] = name;
  cb_do_callback(&in->cb, TAB_CHANGED);
}

void tab_next(tab_in_t* in) {
  if (in->num_tabs == 0) {
    return;
  }
  in->active_tab = (in->active_tab + 1) % in->num_tabs;
  cb_do_callback(&in->cb, TAB_CHANGED);
}

void tab_previous(tab_in_t* in) {
  if (in->num_tabs == 0) {
    return;
  }
  if (in->active_tab == 0) {
    in->active_tab = in->num_tabs;
  }
  in->active_tab = (in->active_tab - 1) % in->num_tabs;
  cb_do_callback(&in->cb, TAB_CHANGED);
}

char** tab_get_names(tab_in_t* in) {
  return in->tab_names;
}

size_t tab_get_index(tab_in_t* in) {
  return in->active_tab;
}

size_t tab_get_num_names(tab_in_t* in) {
  return in->num_tabs;
}
