#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <fcntl.h>
#include <unistd.h>

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
bc_getbigcharpos (int *big, int x, int y, int *value)
{
  if (x < 0 || y < 0 || y > 7 || x > 7 || big == NULL || value == NULL)
    return -1;

  if (x * 8 + y <= 31)
    *value = (*big >> (31 - (x * 8) - y)) & 1;
  else 
    *value = (*(big + 1) >> (31 - (x * 8) - y)) & 1;

  return 0;
}


int
bc_printbigchar (int *big, int x, int y)
{
  if (big == NULL || x < 0 || y < 0)
    return -1;

  int fd = open ("/dev/stdout", O_WRONLY);
  if (fd == -1)
    return -1;

  for (int i = 0; i <= 7; i++)
    {
      mt_gotoXY (x, y + i);
      for (int j = 0, bit, size; j <= 7; j++)
        {
          bc_getbigcharpos(big, i, j, &bit);
          size = bit == 1 ? 3 : 1;
          write (fd, bit == 1 ? "\u2592" : " ", size);
        }
    }
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

  if (x * 8 + y <= 31)
    *big |= (value << (31 - (x * 8) - y)); 
  else
    *(big + 1) |= (value << (31 - (x * 8) - y)); 
  
  return 0;
}

int main()
{
    int c[2] = {0, 0};

    bc_setbigcharpos(c, 7, 7, 1);
    bc_setbigcharpos(c, 5, 4, 1);
    bc_setbigcharpos(c, 3, 2, 1);

    int value = 0;

    bc_printbigchar(c, 7, 7);

    printf("%d\n", value);

/*
    for (int x = 0; x <= 7; x++)
    {

        for (int y = 0; y <= 7; y++)
        {
            int value = 0;
            bc_getbigcharpos(c, x, y, &value);
            printf("%d", value);
        }
        printf("\n");
    } */
}

