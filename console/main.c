#include <stdio.h>

#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

#include "myBigChars.h"
#include "myReadKey.h"
#include "mySimpleComputer.h"
#include "print.h"

int
main (int argv, char *argc[])
{
  int big[36], fd, count;

  if (argv >= 2)
    {
      fd = open (argc[1], O_RDONLY);
      if (fd == -1)
        {
          printf ("Не удается открыть файл, указанный в параметрах\n");
          return -1;
        }
    }
  else
    {
      fd = open ("font.bin", O_RDONLY);
      if (fd == -1)
        {
          printf ("Не удается найти/открыть файл font.bin\n");
          return -1;
        }
    }
  bc_bigcharread (fd, big, 36, &count);

  close (fd);

  fd = open ("/dev/stdout", O_RDWR);
  if (fd == -1)
    {
      printf ("Ошибка! Стандартный поток вывода закрыт\n");
      return 1;
    }
  close (fd);

  int rows, cols;

  mt_getscreensize (&rows, &cols);
  if (rows < 27 || cols < 108)
    {
      printf ("Ошибка! Увеличьте размер терминала\n");
      return 1;
    }

  sc_memoryInit ();
  sc_regInit ();
  sc_accumulatorInit ();
  sc_icounterInit ();

  mt_clrscr ();
  mt_setcursorvisible (1);

  for (int i = 0; i < 10; i++)
    sc_memorySet (i * 10, i % 2 == 0 ? i * 15 : i * 30);

  sc_memorySet (8, __SHRT_MAX__);
  sc_icounterSet (8);

  for (int i = 0; i < 128; i++)
    printCell (i, DEFAULT, DEFAULT);

  int value;
  sc_memoryGet (8, &value);

  printFlags ();
  printDecodedCommand (value);
  printAccumulator ();
  printCounters ();
  printCommand ();

  bc_box (1, 1, 60, 14, DEFAULT, FG_WHITE, " Оперативная память ", BG_RED,
          DEFAULT);
  bc_box (1, 16, 60, 2, DEFAULT, FG_WHITE, " Редактируемая ячейка (формат) ",
          BG_RED, FG_WHITE);
  bc_box (62, 1, 22, 2, DEFAULT, FG_WHITE, " Аккумулятор ", BG_RED, DEFAULT);
  bc_box (85, 1, 24, 2, DEFAULT, FG_WHITE, " Регистр флагов ", BG_RED,
          DEFAULT);
  bc_box (62, 4, 22, 2, DEFAULT, FG_WHITE, " Счетчик команд ", BG_RED,
          DEFAULT);
  bc_box (85, 4, 24, 2, DEFAULT, FG_WHITE, " Команда ", BG_RED, DEFAULT);
  bc_box (62, 7, 47, 11, DEFAULT, FG_WHITE,
          " Редактируемая ячейка (увеличено) ", BG_RED, FG_WHITE);
  bc_box (68, 19, 10, 6, DEFAULT, FG_WHITE, " IN--OUT ", BG_GREEN, FG_WHITE);
  bc_box (79, 19, 30, 6, DEFAULT, FG_WHITE, " Клавиши ", BG_GREEN, FG_WHITE);
  bc_box (1, 19, 66, 6, DEFAULT, FG_WHITE, " Кеш процессора ", BG_GREEN,
          FG_WHITE);

  unsigned int address = 0;
  enum keys key;
  struct termios t;

  tcgetattr (STDIN_FILENO, &t);

  printCell (address, FG_WHITE, BG_BLACK);
  printBigCell (address, big);

  sc_memoryGet (address, &value);
  printDecodedCommand (value);

  if (rk_mytermregime (0, 0, 0, 0, 0))
    return -1;

  while (1)
    {
      if (rk_readkey (&key))
        return -1;

      if (key == key_esape)
        break;

      switch (key)
        {
        case key_enter:
          rk_readvalue (&value, 100);
          sc_memorySet (address, value);
          break;

        case key_up:
          printCell (address, DEFAULT, DEFAULT);
          address -= 10;
          if (address > 127)
            {
              address = (address % 128) + 2;
              if (address == 128 || address == 129)
                address -= 10;
            }
          break;

        case key_down:
          printCell (address, DEFAULT, DEFAULT);
          address += 10;
          if (address >= 128 && address < 130)
            address = 8 + (address & 1);
          else if (address >= 130)
            address = (address & 0XF) - 2;
          break;

        case key_left:
          printCell (address, DEFAULT, DEFAULT);
          address--;
          address %= 128;
          break;

        case key_right:
          printCell (address, DEFAULT, DEFAULT);
          address++;
          address %= 128;
          break;

        default:
          break;
        };
      printCell (address, FG_WHITE, BG_BLACK);
      // printBigCell (address, big);
      // sc_memoryGet (address, &value);
      // printDecodedCommand (value);
    }

  tcsetattr (STDIN_FILENO, TCSAFLUSH, &t);
  mt_setcursorvisible (0);
  mt_gotoXY (0, 27);

  return 0;
}