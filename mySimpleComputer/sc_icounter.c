#include <stdlib.h>

#include "mySimpleComputer.h"

int sc_icounter;

int
sc_icounterInit ()
{
  return (sc_icounter = 0);
}

int
sc_icounterSet (int value)
{
  if (__SHRT_MAX__ < value || value < 0)
    return -1;
  sc_icounter = value;
  return 0;
}

int
sc_icounterGet (int *value)
{
  if (value == NULL)
    return -1;
  *value = sc_icounter;
  return 0;
}