#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <signal.h>
#include <sys/time.h>
#include <unistd.h>

#include "console/print.h"

#include "myReadKey.h"
#include "mySimpleComputer.h"
#include "myTerm.h"

#define COUNT_PULSE_MEMORY 5

int tact_counter = 0;
int flag_IRC_run = 0;

enum
{
  TACT = SIGALRM,
  TACT_T = SIGUSR2,
  RESET = SIGUSR1,
};

struct itimerval nval, oval;

void
init_generator ()
{
  /* generate 2 pulse per second */
  nval.it_interval.tv_sec = 0;
  nval.it_interval.tv_usec = 500000;
  nval.it_value.tv_sec = 0;
  nval.it_value.tv_usec = 500000;
}

int
memoryController (int operand, int *value, int state)
{
  signal (TACT, IRC);

  if (value == NULL)
    return -1;
  tact_counter = COUNT_PULSE_MEMORY;

  int temp_value;

  while (1)
    {
      pause ();
      printTactCounter (tact_counter);

      if (flag_IRC_run)
        {
          flag_IRC_run = 0;
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
          break;
        }
    }
  return 0;
}

int
IO (int command, int operand)
{
  int value;
  char buf[4];

  switch (command)
    {
    case CPUINFO:
      signal (TACT, IRC);
      mt_gotoXY (0, 26);
      write (STDOUT_FILENO, "Носов Денис Алексеевич, ИВ-323", 52);
      tact_counter += 8;
      while (1)
        {
          pause ();
          printTactCounter (tact_counter);
          if (flag_IRC_run)
            {
              flag_IRC_run = 0;
              break;
            }
        }
      mt_gotoXY (0, 26);
      mt_delline ();
      break;

    case READ:
      alarm (0);
      snprintf (buf, 4, "%02d<", operand);
      mt_gotoXY (69, 24);
      write (STDOUT_FILENO, buf, 4);
      mt_gotoXY (73, 24);
      rk_readvalue (&value, 300);
      setitimer (ITIMER_REAL, &nval, &oval);
      memoryController (operand, &value, SET);
      printCell (operand, DEFAULT, DEFAULT);
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

void
run_simplecomputer ()
{
  int flag_ignore_pulse = 0, registr;

  sc_regGet (3, &flag_ignore_pulse);
  if (flag_ignore_pulse)
    return;

  init_generator ();
  signal (TACT, IRC);
  setitimer (ITIMER_REAL, &nval, &oval);

  while (1)
    {
      pause ();
      printTactCounter (tact_counter);
      if (flag_IRC_run)
        {
          flag_IRC_run = 0;
          CU ();
          for (int i = 0; i < 5; i++)
            {
              sc_regGet (i, &registr);
              if (registr)
                break;
            }
          if (registr)
            break;
        }
    }
  alarm (0);
  sc_regSet (3, 1);
}

void
IRC (int signum)
{
  signal (RESET, CU_reset);
  signal (TACT_T, CU);

  switch (signum)
    {
    case TACT:
      if (tact_counter > 0)
        tact_counter--;
      else
        flag_IRC_run = 1;
      break;
    case RESET:
      raise (signum);
      break;
    case TACT_T:
      init_generator ();
      setitimer (ITIMER_REAL, &nval, &oval);
      raise (signum);
      alarm (0);
      break;
    default:
      break;
    }
}

void
CU_reset ()
{
  sc_memoryInit ();
  sc_regInit ();
  sc_accumulatorSet (0);
  sc_icounterSet (0);
  sc_regSet (3, 1);
  tact_counter = 0;
}

void
CU ()
{
  int sign, command, operand;
  int accumulator, value_memory;
  int i;

  sc_icounterGet (&i);
  printCounters ();
  if (memoryController (i, &value_memory, GET))
    {
      sc_regSet (2, 1);
      return;
    }
  printCommand ();
  i++;

  if (sc_commandDecode (value_memory, &sign, &command, &operand)
      || sc_commandValidate (command) || sign == 1)
    {
      sc_regSet (4, 1);
      printCommand ();
      return;
    }
  switch (command)
    {
    case NOP:
      break;
    case CPUINFO:
    case READ:
    case WRITE:
      IO (command, operand);
      break;
    case LOAD:
      if (memoryController (operand, &value_memory, GET))
        {
          sc_regSet (2, 1);
          break;
        }
      sc_accumulatorSet (value_memory);
      printAccumulator ();
      break;
    case STORE:
      sc_accumulatorGet (&accumulator);
      if (memoryController (operand, &accumulator, SET))
        {
          sc_regSet (2, 1);
          break;
        }
      printCell (operand, DEFAULT, DEFAULT);
      break;
    case ADD:
    case SUB:
    case DIVIDE:
    case MUL:
    case SUBC:
    case LOGLC:
      ALU (command, operand);
      break;
    case JUMP:
    case JNEG:
    case JZ:
      if (0 > operand || operand >= SIZE_MEMORY)
        {
          sc_regSet (2, 1);
          break;
        }
      switch (command)
        {
        case JUMP:
          i = operand;
          break;
        case JNEG:
          sc_accumulatorGet (&accumulator);
          if ((accumulator >> 14) == 1)
            i = operand;
          break;
        case JZ:
          sc_accumulatorGet (&accumulator);
          if (accumulator == 0)
            i = operand;
          break;
        }
      break;
    case HALT:
      sc_regSet (3, 1);
      break;
    default:
      break;
    }
  sc_icounterSet (i);
}

int
ALU (int command, int operand)
{
  int old_accumulator, old_value_memory;
  int accumulator, value_memory;
  char accumulator_char[6], value_char[6];

  sc_accumulatorGet (&accumulator);
  old_accumulator = accumulator;

  if (memoryController (operand, &value_memory, GET))
    {
      sc_regSet (2, 1);
      return -1;
    }
  old_value_memory = value_memory;

  snprintf (accumulator_char, 6, "%05d", (old_accumulator) & ~(1 << 14));
  snprintf (value_char, 6, "%05d", (old_value_memory) & ~(1 << 14));

  accumulator = atoi (accumulator_char);
  value_memory = atoi (value_char);

  if ((old_accumulator >> 14) == 1)
    accumulator *= -1;
  if ((old_value_memory >> 14) == 1)
    value_memory *= -1;

  printf ("%d %d", accumulator, value_memory);

  switch (command)
    {
    case ADD:
      accumulator += value_memory;
      break;
    case SUB:
      accumulator -= value_memory;
      break;
    case DIVIDE:
      if (value_memory == 0)
        {
          sc_regSet (1, 1);
          return -1;
        }
      accumulator /= value_memory;
      break;
    case MUL:
      accumulator *= value_memory;
      break;
    case SUBC:
      sc_accumulatorSet (old_value_memory);
      printAccumulator ();
      return ALU (SUB, old_accumulator);
    case LOGLC:
      value_memory *= (value_memory < 0) ? -1 : 1;
      accumulator = value_memory << accumulator;
      accumulator &= 0x3FFF;
      accumulator *= (old_value_memory >> 14) == 1 ? -1 : 1;
      break;
    default:
      sc_regSet (4, 1);
      printCommand ();
      return -1;
    };
  if (0x3FFF >= accumulator && accumulator >= -0x3FFF)
    {
      if (accumulator < 0)
        {
          accumulator *= -1;
          accumulator |= (1 << 14);
        }
    }
  else
    {
      sc_regSet (0, 1);
      return -1;
    }
  sc_accumulatorSet (accumulator);
  printAccumulator ();
  return 0;
}