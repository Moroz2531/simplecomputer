#ifndef MYTERM_H
#define MYTERM_H

enum colors
{
  BLACK,
  RED,
  GREEN,
  YELLOW,
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