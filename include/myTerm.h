#ifndef MYTERM_H
#define MYTERM_H

enum colors
{
  DEFAULT = 0,

  FG_BLACK = 40,
  FG_RED = 41,
  FG_GREEN = 42,
  FG_YELLOW = 43,
  FG_BLUE = 44,
  FG_WHITE = 47,

  BG_WHITE = 1,
  BG_BLACK = 30,
  BG_RED = 31,
  BG_GREEN = 32,
  BG_YELLOW = 33,
  BG_BLUE = 34,
};

int mt_clrscr ();
int get_count_numbers (int num);
int mt_gotoXY (int x, int y);
int mt_getscreensize (int *rows, int *cols);
int mt_setfgcolor (enum colors color);
int mt_setbgcolor (enum colors color);
int mt_setdefaultcolor ();
int mt_setcursorvisible (int value);
int mt_delline ();

#endif