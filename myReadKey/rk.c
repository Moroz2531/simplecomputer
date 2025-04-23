#include <stdlib.h>
#include <string.h>

#include <termios.h>
#include <unistd.h>

#include "console/print.h"

#include "myBigChars.h"
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
  int value_temp;
  if (sc_commandEncode (command[0], command[1], command[2], &value_temp))
    return -1;
  *value = value_temp;
  return 0;
}

int
file_save_load (enum keys key)
{
  int x;

  mt_gotoXY (0, 26);

  switch (key)
    {
    case key_l:
      write (STDOUT_FILENO, "Введите имя файла для загрузки: ", 58);
      x = bc_strlen ("Введите имя файла для загрузки: ");
      break;
    case key_s:
      write (STDOUT_FILENO, "Введите имя файла для сохранения: ", 62);
      x = bc_strlen ("Введите имя файла для сохранения: ");
      break;
    default:
      return -1;
    }

  char *filename = calloc (sizeof (char), 256);
  if (filename == NULL)
    return -1;

  mt_setcursorvisible (0);
  rk_mytermsave ();

  for (int i = 0, count_byte = 0;;)
    {
      char buf[16] = "\0";

      rk_mytermregime (0, 0, 0, 1, 1);
      count_byte = read (STDIN_FILENO, buf, 15);
      if (strcmp (buf, "\n") == 0 || i >= 254 || count_byte < 0)
        break;
      if (strcmp (buf, "\177") == 0)
        {
          mt_gotoXY (x + 1, 26);
          filename[0] = '\0';
          i = 0;
          write (STDOUT_FILENO, "\e[K", 3);
          continue;
        }

      strcat (filename, buf);
      i += count_byte;
    }
  mt_setcursorvisible (1);
  rk_mytermrestore ();
  switch (key)
    {
    case key_l:
      sc_memoryLoad (filename);
      break;
    case key_s:
      sc_memorySave (filename);
    default:
      break;
    }
  free (filename);
  mt_gotoXY (0, 26);
  mt_delline ();

  return 0;
}

int
move (enum keys key, unsigned int *addr, int *big)
{
  unsigned int address = *addr;
  int value;

  printCell (address, DEFAULT, DEFAULT);
  switch (key)
    {
    case key_up:
      address -= 10;
      if (address > 127)
        {
          address = (address % 128) + 2;
          if (address == 128 || address == 129)
            address -= 10;
        }
      break;

    case key_down:
      address += 10;
      if (address >= 128 && address < 130)
        address = 8 + (address & 1);
      else if (address >= 130)
        address = (address & 0XF) - 2;
      break;

    case key_left:
      address--;
      address %= 128;
      break;

    case key_right:
      address++;
      address %= 128;
      break;

    default:
      break;
    }
  printCell (address, FG_WHITE, BG_BLACK);
  printBigCell (address, big);
  sc_memoryGet (address, &value);
  printDecodedCommand (value);
  *addr = address;
  return 0;
}