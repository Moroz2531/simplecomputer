#include <stdio.h>

#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "mySimpleComputer.h"
#include "print.h"

int
main ()
{
  sc_memoryInit ();
  sc_regInit ();
  sc_accumulatorInit ();
  sc_icounterInit ();

  mt_clrscr ();

  for (int i = 0; i < 10; i++)
    sc_memorySet (i * 10, i % 2 == 0 ? i * 15 : i * 30);

  for (int i = 0; i < 128; i++)
    printCell (i, BLACK, 0);

  printFlags ();
  printDecodedCommand (__SHRT_MAX__);
  printAccumulator ();

  mt_gotoXY (0, 20);

  return 0;
}