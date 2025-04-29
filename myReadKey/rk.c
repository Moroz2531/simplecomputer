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

static int
rk_readvalue_check_sign (char buf[2], int *command)
{
  if (command == NULL)
    return -1;

  if (strcmp (buf, "+") == 0)
    *command = 0;
  else if (strcmp (buf, "-") == 0)
    *command = 1;
  else
    return -1;
  return 0;
}

static int
rk_readvalue_check_value (char buf[2], int *command, int shift)
{
  if (command == NULL)
    return -1;

  switch (shift)
    {
    case 1:
      shift = 4;
      break;
    default:
      shift = 0;
    };

  if (strcmp (buf, "0") == 0)
    return 0;
  else if (strcmp (buf, "1") == 0)
    *command |= (1 << shift);
  else if (strcmp (buf, "2") == 0)
    *command |= (2 << shift);
  else if (strcmp (buf, "3") == 0)
    *command |= (3 << shift);
  else if (strcmp (buf, "4") == 0)
    *command |= (4 << shift);
  else if (strcmp (buf, "5") == 0)
    *command |= (5 << shift);
  else if (strcmp (buf, "6") == 0)
    *command |= (6 << shift);
  else if (strcmp (buf, "7") == 0)
    *command |= (7 << shift);
  else if (strcmp (buf, "8") == 0)
    *command |= (8 << shift);
  else if (strcmp (buf, "9") == 0)
    *command |= (9 << shift);
  else if (strcmp (buf, "A") == 0)
    *command |= (10 << shift);
  else if (strcmp (buf, "B") == 0)
    *command |= (11 << shift);
  else if (strcmp (buf, "C") == 0)
    *command |= (12 << shift);
  else if (strcmp (buf, "D") == 0)
    *command |= (13 << shift);
  else if (strcmp (buf, "E") == 0)
    *command |= (14 << shift);
  else if (strcmp (buf, "F") == 0)
    *command |= (15 << shift);
  else
    return -1;
  return 0;
}

int
rk_readvalue (int *value)
{
  mt_setcursorvisible (0);

  int i = 0, command[3] = { 0 };
  char buf[2];

  while (i < 5)
    {
      int count_read = read (STDIN_FILENO, buf, 1);

      if (count_read < 0)
        return -1;
      buf[count_read] = '\0';

      if (strcmp (buf, "\e") == 0)
        {
          mt_setcursorvisible (1);
          rk_mytermrestore ();
          return 0;
        }

      if (i == 0)
        {
          if (rk_readvalue_check_sign (buf, command))
            continue;
        }
      else
        {
          if (i <= 2)
            {
              if (rk_readvalue_check_value (buf, &command[1], 2 - i))
                continue;
            }
          else if (rk_readvalue_check_value (buf, &command[2], 4 - i))
            continue;
        }
      write (STDOUT_FILENO, buf, 1);
      i++;
    }
  mt_setcursorvisible (1);
  if (sc_commandEncode (command[0], command[1], command[2], value))
    return -1;

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