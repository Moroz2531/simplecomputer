#include <stdio.h>

#include <fcntl.h>
#include <unistd.h>

#include "myBigChars.h"
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
  if (rows < 26 || cols < 108)
    {
      printf ("Ошибка! Увеличьте размер терминала\n");
      return 1;
    }

  sc_memoryInit ();
  sc_regInit ();
  sc_accumulatorInit ();
  sc_icounterInit ();

  mt_clrscr ();

  for (int i = 0; i < 10; i++)
    sc_memorySet (i * 10, i % 2 == 0 ? i * 15 : i * 30);

  sc_memorySet (8, __SHRT_MAX__);
  sc_icounterSet (8);

  for (int i = 0; i < 128; i++)
    printCell (i, BG_WHITE, DEFAULT);

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

  unsigned int address = 8;

  printCell (address, BG_BLACK, FG_WHITE);
  printBigCell (address, big);

  for (int i = 0; i < 7; i++)
    {
      printTerm (i * 10, i % 2);
    }
  mt_gotoXY (0, 26);

  return 0;
}