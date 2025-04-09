#include <string.h>
#include <termios.h>
#include <unistd.h>

#include "myReadKey.h"
#include "mySimpleComputer.h"
#include "myTerm.h"

static struct termios oldt;

int
rk_readkey (enum keys *key)
{
  if (key == NULL || rk_mytermsave () || rk_mytermregime (0, 0, 1, 0, 0))
    return -1;

  char k[16];
  int count_read = read (STDIN_FILENO, k, 15);

  if (count_read < 0)
    return -1;
  k[count_read] = '\0';

  if (rk_mytermrestore ())
    return -1;

  if (strcmp (k, "\e[A") == 0)
    *key = key_up;
  else if (strcmp (k, "\e[B") == 0)
    *key = key_down;
  else if (strcmp (k, "\e[C") == 0)
    *key = key_right;
  else if (strcmp (k, "\e[D") == 0)
    *key = key_left;
  else if (strcmp (k, "\n") == 0)
    *key = key_enter;
  else if (strcmp (k, "\e") == 0)
    *key = key_esape;
  else if (strcmp (k, "\e[15~") == 0)
    *key = key_f5;
  else if (strcmp (k, "\e[17~") == 0)
    *key = key_f6;
  else if (strcmp (k, "l") == 0 || strcmp (k, "L") == 0)
    *key = key_l;
  else if (strcmp (k, "s") == 0 || strcmp (k, "S") == 0)
    *key = key_s;
  else if (strcmp (k, "i") == 0 || strcmp (k, "I") == 0)
    *key = key_i;
  else if (strcmp (k, "r") == 0 || strcmp (k, "R") == 0)
    *key = key__r;
  else if (strcmp (k, "t") == 0 || strcmp (k, "T") == 0)
    *key = key__t;
  else
    *key = key_unknown;

  return 0;
}

int
rk_mytermsave ()
{
  return tcgetattr (STDIN_FILENO, &oldt);
}

int
rk_mytermrestore ()
{
  return tcsetattr (STDIN_FILENO, TCSAFLUSH, &oldt);
}

int
rk_mytermregime (int regime, int vtime, int vmin, int echo, int sigint)
{
  static struct termios t;

  if (tcgetattr (STDIN_FILENO, &t) == -1)
    return -1;

  t.c_lflag &= ~(ICANON | ECHO | ISIG);
  t.c_lflag |= (regime ? ICANON : 0) | (echo ? ECHO : 0) | (sigint ? ISIG : 0);

  if (regime == 0)
    {
      t.c_cc[VMIN] = vmin;
      t.c_cc[VTIME] = vtime;
    }

  return tcsetattr (STDIN_FILENO, TCSAFLUSH, &t);
}

int
rk_readvalue (int *value, int timeout)
{
  if (rk_mytermsave () || rk_mytermregime (0, timeout, 5, 1, 0))
    return -1;

  char buf[6];

  mt_setcursorvisible (0);
  read (STDIN_FILENO, buf, 5);
  mt_setcursorvisible (1);

  if (rk_mytermrestore ())
    return -1;

  int command[3] = { 0, 0, 0 };

  switch (buf[0])
    {
    case '+':
      command[0] = 0;
      break;
    case '-':
      command[0] = 1;
      break;
    default:
      return -1;
    }
  int numbers[16][2] = { { '0', 0 },  { '1', 1 },  { '2', 2 },  { '3', 3 },
                         { '4', 4 },  { '5', 5 },  { '6', 6 },  { '7', 7 },
                         { '8', 8 },  { '9', 9 },  { 'A', 10 }, { 'B', 11 },
                         { 'C', 12 }, { 'D', 13 }, { 'E', 14 }, { 'F', 15 } };

  for (int i = 1, flag_num; i < 5; i++)
    {
      flag_num = 0;
      for (int j = 0; j < 16; j++)
        {
          if (buf[i] == numbers[j][0])
            {
              command[1 + ((i - 1) >> 1)] |= numbers[j][1] << (4 * (i % 2));
              flag_num = 1;
              break;
            }
        }
      if (!flag_num)
        return -1;
    }
  if (sc_commandEncode (command[0], command[1], command[2], value))
    return -1;
  return 0;
}