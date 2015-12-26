#include "tab_model.h"
#include <stdlib.h>
#include <string.h>

struct tab_model_t {
  ed_in_t** tabs;
  size_t num_tabs;
  size_t active_tab;
  cb_t cb;
};

tab_in_t* init_tab_input(controller_call_back_t cb, void* controller) {
  tab_in_t* in = malloc(sizeof(tab_in_t));
  in->num_tabs = 0;
  in->active_tab = 0;
  in->tabs = NULL;
  in->cb.cb = cb;
  in->cb.controller = controller;
  return in;
}

static int exists(tab_in_t* in, char* relative_file_path) {
  for (size_t i = 0; i < in->num_tabs; i++) {
    if (!strcmp(ed_in_get_file_name(in->tabs[i]), relative_file_path)) {
      return 1;
    }
  }
  return 0;
}

void tab_in_free(tab_in_t* in) {
  for (size_t i = 0; i < in->num_tabs; i++) {
    free(in->tabs[i]);
  }
  free(in->tabs);
  free(in);
}

int tab_in_register_tab(tab_in_t* in, char* relative_file_path) {
  if (exists(in, relative_file_path)) {
    return 1;
  }
  in->num_tabs++;
  in->active_tab = in->num_tabs - 1;
  in->tabs = realloc(in->tabs, in->num_tabs * sizeof(ed_in_t*));
  in->tabs[in->active_tab] = init_editor_input(in->cb.cb, in->cb.controller, relative_file_path);
  cb_do_callback(&in->cb, TAB_CHANGED);
  return 0;
}

void tab_in_unregister_tab(tab_in_t* in) {
  if (in->num_tabs == 0) {
    return;
  }
  ed_in_free(in->tabs[in->active_tab]);
  for (size_t i = in->active_tab + 1; i < in->num_tabs; i++) {
    in->tabs[i - 1] = in->tabs[i];
  }
  in->num_tabs--;
  if (in->active_tab != 0) {
    in->active_tab = in->active_tab - 1;
  }
  in->tabs = realloc(in->tabs, in->num_tabs * sizeof(ed_in_t*));
  cb_do_callback(&in->cb, TAB_CLOSED);
}

void tab_next(tab_in_t* in) {
  if (in->num_tabs <= 1) {
    return;
  }
  in->active_tab = (in->active_tab + 1) % in->num_tabs;
  cb_do_callback(&in->cb, TAB_CHANGED);
}

void tab_previous(tab_in_t* in) {
  if (in->num_tabs <= 1) {
    return;
  }
  if (in->active_tab == 0) {
    in->active_tab = in->num_tabs;
  }
  in->active_tab = (in->active_tab - 1) % in->num_tabs;
  cb_do_callback(&in->cb, TAB_CHANGED);
}

ed_in_t** tab_get_tabs(tab_in_t* in) {
  return in->tabs;
}

size_t tab_get_index(tab_in_t* in) {
  return in->active_tab;
}

size_t tab_get_num_names(tab_in_t* in) {
  return in->num_tabs;
}

ed_in_t* tab_get_active_tab(tab_in_t* in) {
  if (!in->num_tabs) {
    return NULL;
  }
  return in->tabs[in->active_tab];
}
