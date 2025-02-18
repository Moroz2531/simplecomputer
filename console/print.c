#include <stdio.h>

#include "mySimpleComputer.h"
#include "print.h"

void
printCell (int address)
{
  int value = 0;
  if (sc_memoryGet (address, &value))
    {
      printf ("Вы вышли за границы оперативной памяти. (%d)\n", address);
      return;
    }
  printf ("Содержимое ячейки %d: ", address);
  printDecodedCommand (value);
}

void
printFlags ()
{
  int value = 0;
  printf ("Значения флагов:\n");
  for (int i = 0; i < 5; i++)
    {
      sc_regGet (i, &value);
      if (value == 0)
        printf ("_");
      else
        printf ("%d", value);
    }
  printf ("\n");
}

void
printDecodedCommand (int value)
{
  printf ("bin: ");
  for (int i = sizeof (value) * 8 - 1, flag = 0; i >= 0; i--)
    {
      if (flag)
        printf ("%d", (value >> i) & 1);
      else if ((value >> i) & 1)
        {
          i++;
          flag = 1;
        }
    }
  printf (" | oct: %o | dec: %d | hex: %x\n", value, value, value);
}

void
printAccumulator ()
{
  int value = 0;
  sc_accumulatorGet (&value);
  printf ("Значение аккумулятора: %d\n", value);
}

void
printCounters ()
{
  int value = 0;
  sc_icounterGet (&value);
  printf ("Значение счётчика команд: %d\n", value);
}