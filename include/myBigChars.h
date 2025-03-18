#ifndef MYBIGCHARS_H
#define MYBIGCHARS_H

#include "myTerm.h"

int bc_strlen (char *str);

int bc_box (int x1, int y1, int x2, int y2, enum colors box_fg,
            enum colors box_bg, char *header, enum colors header_fg,
            enum colors headers_bg);

int bc_bigcharread (int fd, int *big, int need_count, int *count);
int bc_bigcharwrite (int fd, int *big, int count);

int bc_getbigcharpos (int *big, int x, int y, int *value);
int bc_setbigcharpos (int *big, int x, int y, int value);
int bc_printbigchar (int *big, int x, int y, enum colors bg, enum colors fg);

#endif