#ifndef _FONTOPTIONS_H
#define _FONTOPTIONS_H

#define NO_OPTION -1

#define FONT_BOLD_MASK 0x3
#define FONT_BRIGHT_MASK 0xC
#define FONT_UNDERSCORE_MASK 0x30
#define FONT_BLINK_MASK 0xC0

enum FONT_OPTION {
  FONT_DEFAULT =           0x0,
  FONT_BOLD_ON =           0x1,
  FONT_BOLD_OFF =          0x2,
  FONT_HALF_BRIGHT_ON =    0x4,
  FONT_HALF_BRIGHT_OFF =   0x8,
  FONT_UNDERSCORE_ON =     0x10,
  FONT_UNDERSCORE_OFF =    0x20,
  FONT_BLINK_ON =          0x40,
  FONT_BLINK_OFF =         0x80
};

enum FOREGROUND_COLORS {
  FG_BLACK =          30,
  FG_RED =            31,
  FG_GREEN =          32,
  FG_BROWN =          33,
  FG_BLUE =           34,
  FG_MAGENTA =        35,
  FG_CYAN =           36,
  FG_WHITE =          37
};

enum BACKGROUND_COLORS {
  BG_BLACK =          40,
  BG_RED =            41,
  BG_GREEN =          42,
  BG_BROWN =          43,
  BG_BLUE =           44,
  BG_MAGENTA =        45,
  BG_CYAN =           46,
  BG_WHITE =          47
};

#define IS_FOREGROUND(x) (30 <= x && x <= 37)
#define IS_BACKGROUND(x) (40 <= x && x <= 47)

#endif
