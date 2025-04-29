#include <stdlib.h>

#include "mySimpleComputer.h"

#define SC_REGSET(x, y) y << x
#define SC_REGGET(x, y) (y >> x) & 1

int sc_register;

int
sc_regInit ()
{
  return (sc_register = 0);
}

int
sc_regSet (int registr, int value)
{
  if (1 < value || value < 0 || 4 < registr || registr < 0)
    return -1;
  if (value == 1)
    sc_register |= SC_REGSET (registr, value);
  else
    sc_register &= ~(1 << registr);
  return 0;
}

int
sc_regGet (int registr, int *value)
{
  if (4 < registr || registr < 0 || value == NULL)
    return -1;
  *value = SC_REGGET (registr, sc_register);
  return 0;
}