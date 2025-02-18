#include <stdio.h>

#include "mySimpleComputer.h"
#include "print.h"

int
main ()
{
  sc_memoryInit ();
  sc_regInit ();
  sc_accumulatorInit ();
  sc_icounterInit ();

  for (int i = 0; i < 10; i++)
    sc_memorySet (i * 10, i % 2 == 0 ? i * 15 : i * 30);

  for (int i = 0; i < 10; i++)
    printCell (i * 10);

  printCell (-1);
  printCell (120);
  printCell (128);

  printf ("Статус завершения функции sc_memorySet: %d\n",
          sc_memorySet (128, 50));
  printf ("Статус завершения функции sc_memorySet: %d\n",
          sc_memorySet (127, -1));

  printFlags ();
  sc_regSet (0, 1);
  sc_regSet (4, 1);
  printFlags ();
  for (int i = 0; i < 5; i++)
    {
      sc_regSet (i, 1);
    }
  printFlags ();

  printf ("Статус завершения функции sc_regSet: %d\n", sc_regSet (5, 0));
  printf ("Статус завершения функции sc_regSet: %d\n", sc_regSet (4, -1));
  printf ("Статус завершения функции sc_regSet: %d\n", sc_regSet (4, 2));

  printAccumulator ();
  sc_accumulatorSet (503);
  printAccumulator ();

  printf ("Статус завершения функции sc_accumulatorSet: %d\n",
          sc_accumulatorSet (50000));

  printCounters ();
  sc_icounterSet (28000);
  printCounters ();

  printf ("Статус завершения функции sc_icounterSet: %d\n",
          sc_icounterSet (50000));

  int value1 = 0;
  int sign = 0, command = 0, operand = 0;

  sc_memoryGet (90, &value1);
  sc_commandDecode (value1, &sign, &command, &operand);
  printf ("%d %d %d\n", sign, command, operand);
  printDecodedCommand (value1);

  sc_accumulatorGet (&value1);
  sc_commandDecode (value1, &sign, &command, &operand);
  printf ("%d %d %d\n", sign, command, operand);
  printDecodedCommand (value1);

  sc_commandEncode (1, 69, 81, &value1);
  sc_commandDecode (value1, &sign, &command, &operand);
  printf ("%d %d %d\n", sign, command, operand);
  printDecodedCommand (value1);

  printf ("%d\n", sc_commandValidate (76));

  sc_memorySave ("memSave");

  return 0;
}