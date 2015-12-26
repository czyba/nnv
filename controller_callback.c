#include "controller_callback.h"
#include "controller.h"

void cb_do_callback(cb_t* cb, enum CALLBACK_TYPE type) {
  cb->cb((c_t*) cb->controller, type);
}
