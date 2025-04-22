#include <stdlib.h>

#include "memory-controller.h"

#include "mySimpleComputer.h"

int
memoryController (int operand, int *value, int state)
{
  if (value == NULL)
    return -1;

  int temp_value;

  clock_pulse_generator (3);

  switch (state)
    {
    case SET:
      if (sc_memorySet (operand, *value))
        return -1;
      break;
    case GET:
      if (sc_memoryGet (operand, &temp_value))
        return -1;
      break;
    default:
      return -1;
    };
  *value = temp_value;
  return 0;
}