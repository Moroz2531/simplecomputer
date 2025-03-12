#include <stdio.h>

#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "myTerm.h"

int
mt_clrscr ()
{
  int fd = open ("/dev/stdout", O_WRONLY);
  if (fd == -1)
    return -1;

  if (write (fd, "\e[H\e[J", 6) == -1)
    return -1;
  close (fd);

  return 0;
}

int
get_count_numbers (int num)
{
  int res = (num == 0) ? 1 : 0;
  while (num > 0)
    {
      num /= 10;
      res++;
    }
  return res;
}

int
mt_gotoXY (int x, int y)
{
  int fd = open ("/dev/stdout", O_WRONLY);
  if (fd == -1 || x < 0 || y < 0)
    return -1;

  const int count_x = get_count_numbers (x);
  const int count_y = get_count_numbers (y);
  char buf_x[count_x + 1], buf_y[count_y + 1];

  snprintf (buf_x, count_x + 2, "%dH", x);
  snprintf (buf_y, count_y + 2, "%d;", y);

  if (write (fd, "\e[", 2) == -1)
    return -1;
  if (write (fd, buf_y, count_y + 1) == -1)
    return -1;
  if (write (fd, buf_x, count_x + 1) == -1)
    return -1;
  close (fd);

  return 0;
}

int
mt_getscreensize (int *rows, int *cols)
{
  if (!isatty (1) || rows == NULL || cols == NULL)
    return -1;

  struct winsize w;
  if (ioctl (1, TIOCGWINSZ, &w) == -1)
    return -1;

  *rows = w.ws_row;
  *cols = w.ws_col;

  return 0;
}

int
mt_setfgcolor (enum colors color)
{
  int fd = open ("/dev/stdout", O_WRONLY);
  if (fd == -1)
    return -1;

  const int count_color = get_count_numbers (color);
  char buf[count_color + 2];

  snprintf (buf, count_color + 2, "%dm", color);

  if (write (fd, "\e[", 2) == -1)
    return -1;
  if (write (fd, buf, count_color + 1) == -1)
    return -1;
  close (fd);

  return 0;
}

int
mt_setbgcolor (enum colors color)
{
  int fd = open ("/dev/stdout", O_WRONLY);
  if (fd == -1)
    return -1;

  const int count_color = get_count_numbers (color);
  char buf[count_color + 2];

  snprintf (buf, count_color + 2, "%dm", color);

  if (write (fd, "\e[;", 2) == -1)
    return -1;
  if (write (fd, buf, count_color + 1) == -1)
    return -1;
  close (fd);

  return 0;
}

int
mt_setdefaultcolor ()
{
  int fd = open ("/dev/stdout", O_WRONLY);
  if (fd == -1)
    return -1;

  if (write (fd, "\e[0m", 4) == -1)
    return -1;
  close (fd);

  return 0;
}

int
mt_setcursorvisible (int value)
{
  int fd = open ("/dev/stdout", O_WRONLY);
  if (fd == -1)
    return -1;

  int size_buf = value ? 6 : 9;
  if (write (fd, value ? "\e[?25l" : "\e[?12;25h", size_buf) == -1)
    return -1;
  close (fd);

  return 0;
}

int
mt_delline ()
{
  int fd = open ("/dev/stdout", O_WRONLY);
  if (fd == -1)
    return -1;

  if (write (fd, "\e[K", 4) == -1)
    return -1;
  close (fd);

  return 0;
}