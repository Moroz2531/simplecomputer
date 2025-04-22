#include <stdio.h>

#include <fcntl.h>
#include <signal.h>
#include <termios.h>
#include <unistd.h>

#include "myBigChars.h"
#include "myReadKey.h"
#include "mySimpleComputer.h"
#include "print.h"

int
main (int argv, char *argc[])
{
  int fd, count;
  int big[36];

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
  sc_regSet (3, 1);

  mt_clrscr ();
  mt_setcursorvisible (1);

  for (int i = 0; i < SIZE_MEMORY; i++)
    printCell (i, DEFAULT, DEFAULT);

  printFlags ();
  printAccumulator ();
  printCounters ();
  printTactCounter (0);
  printCommand ();
  printHelpInformation ();

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
  int value;

  tcgetattr (STDIN_FILENO, &t);

  printCell (address, FG_WHITE, BG_BLACK);
  printBigCell (address, big);

  sc_memoryGet (address, &value);
  printDecodedCommand (value);

  if (rk_mytermregime (0, 0, 0, 1, 1))
    return -1;

  while (1)
    {
      if (rk_readkey (&key))
        return -1;

      if (key == key_esape)
        break;

      switch (key)
        {
        case key_up:
        case key_down:
        case key_left:
        case key_right:
          move (key, &address, big);
          break;

        case key_enter:
          printClearCell (2 + (address % 10 * 6), 2 + address / 10);
          sc_memoryGet (address, &value);
          rk_readvalue (&value, 300);
          sc_memorySet (address, value);
          sc_memoryGet (address, &value);
          printDecodedCommand (value);
          printCell (address, FG_WHITE, BG_BLACK);
          printBigCell (address, big);
          break;

        case key_f5:
          printClearCell (68, 2);
          sc_accumulatorGet (&value);
          rk_readvalue (&value, 300);
          sc_accumulatorSet (value);
          printAccumulator ();
          break;

        case key_f6:
          printClearCell (77, 5);
          sc_icounterGet (&value);
          rk_readvalue (&value, 300);
          sc_icounterSet (value);
          printCounters ();
          printAccumulator ();
          break;

        case key_l:
          file_save_load (key_l);
          for (int i = 0; i < SIZE_MEMORY; i++)
            printCell (i, DEFAULT, DEFAULT);
          sc_memoryGet (address, &value);
          printDecodedCommand (value);
          printCell (address, FG_WHITE, BG_BLACK);
          printBigCell (address, big);
          break;

        case key_s:
          file_save_load (key_s);
          break;

        case key_i:
          IRC (SIGUSR1);
          printFlags ();
          printCommand ();
          printCounters ();
          printTactCounter (0);
          for (int i = 0; i < SIZE_MEMORY; i++)
            printCell (i, DEFAULT, DEFAULT);
          printDecodedCommand (0);
          printCell (address, FG_WHITE, BG_BLACK);
          printBigCell (address, big);
          break;

        case key__r:
          sc_regSet (3, 0);
          printFlags ();
          CU ();
          sc_regSet (3, 1);
          printFlags ();
          break;
        case key__t:
          break;

        default:
          break;
        };
    }
  tcsetattr (STDIN_FILENO, TCSAFLUSH, &t);
  mt_setcursorvisible (0);
  mt_gotoXY (0, 27);

  return 0;
}