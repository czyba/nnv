#ifndef TERMINAL_OUT_OPTIONS_H
#define TERMINAL_OUT_OPTIONS_H

namespace nnv {

enum class FONT_OPTIONS : unsigned char {
  FONT_DEFAULT = 0,
  FONT_BOLD_ON = 1,
  FONT_BOLD_OFF = 21,
  FONT_BRIGHT_ON = 2,
  FONT_BRIGHT_OFF = 22,
  FONT_UNDERSCORE_ON = 4,
  FONT_UNDERSCORE_OFF = 24,
  FONT_BLINK_ON = 5,
  FONT_BLINK_OFF = 25,
  FG_BLACK = 30,
  FG_RED = 31,
  FG_GREEN = 32,
  FG_BROWN = 33,
  FG_BLUE = 34,
  FG_MAGENTA = 35,
  FG_CYAN = 36,
  FG_WHITE = 37,
  BG_BLACK = 40,
  BG_RED = 41,
  BG_GREEN = 42,
  BG_BROWN = 43,
  BG_BLUE = 44,
  BG_MAGENTA = 45,
  BG_CYAN = 46,
  BG_WHITE = 47
};
}

#endif
