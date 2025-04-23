#include <unistd.h>

#include "IO-controller.h"

#include "console/print.h"
#include "memory-controller.h"

#include "myReadKey.h"
#include "mySimpleComputer.h"
#include "myTerm.h"

int
IO (int command, int operand)
{
  if (operand == 0)
    return 0;

  switch (command)
    {
    case CPUINFO:
      mt_gotoXY (0, 26);
      write (STDOUT_FILENO, "Носов Денис Алексеевич, ИВ-323", 52);
      clock_pulse_generator (5);
      mt_gotoXY (0, 26);
      mt_delline ();
      break;
    case READ:
      printTerm (operand, 1);
      break;
    case WRITE:
      printTerm (operand, 0);
      break;
    default:
      return -1;
    }
  return 0;
}