

#define NO_OPTION -1

enum FONT_OPTION {
  DEFAULT =           0,
  BOLD_ON =           1,
  HALF_BRIGHT_ON =    2,
  UNDERSCORE_ON =     4,
  BLINK_ON =          5,
  BOLD_OFF =         21,
  HALF_BRIGHT_OFF =  22,
  UNDERSCORE_OFF =   24,
  BLINK_OFF =        25,
};

#define IS_FONT_OPTION(x) ((0 <= x && x <= 5 && x != 3) || (21 <= x && x <= 25 && x != 23))

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

enum BACKGROUND_COLOR {
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
