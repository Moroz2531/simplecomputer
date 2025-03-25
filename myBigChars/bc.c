#include <string.h>

#include <fcntl.h>
#include <unistd.h>

#include "myBigChars.h"
#include "myTerm.h"

#define ROWS 8
#define COLS 8

int
bc_strlen (char *str)
{
  int count = 0;

  if (str != NULL)
    {
      int limit[2] = { 1040, 1103 }; // Russian 'А' and 'я'
      int numUnicode = 0;

      for (int i = 0, flagUnicode = 0; str[i] != '\0'; i++)
        {
          if ((flagUnicode % 2 == 1) && str[i] >= 0)
            return 0;

          if (str[i] == ' ' || str[i] == '-'
              || ('0' <= str[i] && str[i] <= '9'))
            count++;
          else if (('a' <= str[i] && str[i] <= 'z')
                   || ('A' <= str[i] && str[i] <= 'Z'))
            count++;
          else if (str[i] < 0)
            {
              numUnicode
                  |= (flagUnicode % 2) == 1 ? str[i] & 63 : (str[i] & 31) << 6;

              if ((flagUnicode % 2 == 1) && (limit[0] <= numUnicode)
                  && (numUnicode <= limit[1]))
                {
                  count++;
                  numUnicode = 0;
                }
              flagUnicode++;
            }
          else
            return 0;
        }
    }
  return count;
}

int
bc_box (int x1, int y1, int x2, int y2, enum colors box_fg, enum colors box_bg,
        char *header, enum colors header_fg, enum colors headers_bg)
{
  int fd = open ("/dev/stdout", O_WRONLY);
  if (fd == -1)
    return -1;

  mt_setbgcolor (box_bg);
  mt_setfgcolor (box_fg);
  mt_gotoXY (x1, y1);
  write (fd, "\u250C", 3);

  for (int i = 0; i < x2 - 1; i++)
    write (fd, "\u2500", 3);
  write (fd, "\u2510", 3);

  mt_gotoXY (x1, y1 + y2);
  write (fd, "\u2514", 3);

  for (int i = 0; i < x2 - 1; i++)
    write (fd, "\u2500", 3);
  write (fd, "\u2518", 3);

  for (int i = y1 + 1; i < y1 + y2; i++)
    {
      mt_gotoXY (x1, i);
      write (fd, "\u2502", 3);
      mt_gotoXY (x1 + x2, i);
      write (fd, "\u2502", 3);
    }

  int countSymbol = bc_strlen (header);

  if (countSymbol != 0)
    {
      mt_setbgcolor (headers_bg);
      mt_setfgcolor (header_fg);
      mt_gotoXY (x1 + (x2 / 2) - countSymbol / 2, y1);
      write (fd, header, strlen (header));
    }
  mt_setdefaultcolor ();
  close (fd);

  return 0;
}

int
bc_setbigcharpos (int *big, int x, int y, int value)
{
  if (x < 0 || y < 0 || y > 7 || x > 7 || big == NULL)
    return -1;
  if (value != 0 && value != 1)
    return -1;

  if (value == 1)
    {
      if (x * 8 + y <= 31)
        *big |= (value << (31 - (x * 8) - y));
      else
        *(big + 1) |= (value << (63 - (x * 8) - y));
    }
  else
    {
      if (x * 8 + y <= 31)
        *big &= ~(1 << (31 - (x * 8) - y));
      else
        *(big + 1) &= ~(1 << (63 - (x * 8) - y));
    }
  return 0;
}

int
bc_getbigcharpos (int *big, int x, int y, int *value)
{
  if (x < 0 || y < 0 || y > 7 || x > 7 || big == NULL || value == NULL)
    return -1;

  if (x * 8 + y <= 31)
    *value = (*big >> (31 - (x * 8) - y)) & 1;
  else
    *value = (*(big + 1) >> (63 - (x * 8) - y)) & 1;

  return 0;
}

int
bc_printbigchar (int *big, int x, int y, enum colors bg, enum colors fg)
{
  if (big == NULL || x < 0 || y < 0)
    return -1;

  int fd = open ("/dev/stdout", O_WRONLY);
  if (fd == -1)
    return -1;

  mt_setbgcolor (bg);
  mt_setfgcolor (fg);

  for (int i = 0; i <= 7; i++)
    {
      mt_gotoXY (x, y + i);
      for (int j = 0, bit, size; j <= 7; j++)
        {
          bc_getbigcharpos (big, i, j, &bit);
          size = bit == 1 ? 3 : 1;
          write (fd, bit == 1 ? "\u2592" : " ", size);
        }
    }

  mt_setdefaultcolor ();
  close (fd);

  return 0;
}

int
bc_bigcharwrite (int fd, int *big, int count)
{
  if (fd < 0 || count < 0 || big == NULL)
    return -1;

  if (write (fd, big, count * sizeof (int)) == -1)
    return -1;

  return 0;
}

int
bc_bigcharread (int fd, int *big, int need_count, int *count)
{
  if (fd < 0 || big == NULL || count == NULL || need_count < 0)
    return -1;

  ssize_t size = read (fd, big, need_count * sizeof (int));
  if (size < 0)
    return -1;

  *count = size;

  return 0;
}