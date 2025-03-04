#include <stdio.h>

#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "mySimpleComputer.h"
#include "print.h"

int
main ()
{
  int fd = open ("/dev/stdout", O_RDWR);
  if (fd == -1)
    {
      printf ("Ошибка! Стандартный поток вывода закрыт\n");
      return 1;
    }
  close (fd);

  int rows, cols;

  mt_getscreensize (&rows, &cols);
  if (rows < 26 || cols < 104)
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
  printCell (8, BG_BLACK, FG_WHITE);

  mt_setbgcolor (BG_WHITE);

  int value;
  sc_memoryGet (8, &value);

  printFlags ();
  printDecodedCommand (value);
  printAccumulator ();
  printCounters ();
  printCommand ();

  for (int i = 0; i < 7; i++)
    {
      printTerm (i * 10, i % 2);
    }
  mt_gotoXY (0, 26);

  return 0;
}