#include <stdlib.h>

#include "mySimpleComputer.h"

int sc_accumulator;

int
sc_accumulatorInit ()
{
  return (sc_accumulator = 0);
}

int
sc_accumulatorSet (int value)
{
  if (__SHRT_MAX__ < value || value < 0)
    return -1;
  sc_accumulator = value;
  return 0;
}

int
sc_accumulatorGet (int *value)
{
  if (value == NULL)
    return -1;
  *value = sc_accumulator;
  return 0;
}