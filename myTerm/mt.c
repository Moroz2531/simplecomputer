#include <fcntl.h>
#include <stdio.h>
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

  snprintf (buf_x, count_x + 1, "%d", x);
  snprintf (buf_y, count_y + 1, "%d", y);

  char res[count_x + count_y + 5];
  int i = 0;

  res[i++] = '\e';
  res[i++] = '[';
  for (int j = 0; j < count_x; j++, i++)
    res[i] = buf_x[j];
  res[i++] = ';';
  for (int j = 0; j < count_y; j++, i++)
    res[i] = buf_y[j];
  res[i++] = 'H';
  res[i] = '\0';

  if (write (fd, res, count_x + count_y + 4) == -1)
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
  char buf[count_color + 1];

  snprintf (buf, count_color + 1, "%d", color);

  char res[count_color + 3];
  int i = 0;

  res[i++] = '\e';
  res[i++] = '[';
  for (int j = 0; j < count_color; j++, i++)
    res[i] = buf[j];
  res[i] = '\0';

  if (write (fd, res, count_color + 2) == -1)
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
  char buf[count_color + 1];

  snprintf (buf, count_color + 1, "%d", color);

  char res[count_color + 4];
  int i = 0;

  res[i++] = '\e';
  res[i++] = '[';
  res[i++] = ';';
  for (int j = 0; j < count_color; j++, i++)
    res[i] = buf[j];
  res[i] = '\0';

  if (write (fd, res, count_color + 3) == -1)
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

  if (write (fd, "\e[2K", 4) == -1)
    return -1;
  close (fd);

  return 0;
}