#include <signal.h>
#include <sys/time.h>
#include <unistd.h>

#include "IO-controller.h"
#include "console/print.h"
#include "memory-controller.h"

#include "myReadKey.h"
#include "mySimpleComputer.h"
#include "myTerm.h"

void CU_reset ();

int tact_counter = 0; // счетчик тактов простоя процессора
int flag_not_ignore_tact = 0;

enum
{
  TACT = SIGALRM,
  RESET = SIGUSR1,
};

/* Контроллер прерываний */
/* Генератор импульсов: SIGALRM, от Reset: SIGUSR1 */
void
IRC (int signum)
{
  signal (RESET, CU_reset);
  switch (signum)
    {
    case TACT:
      tact_counter--;
      break;
    case RESET:
      raise (RESET);
      break;
    default:
      break;
    }
}

/* Генератор импульсов */
void
clock_pulse_generator (int tact)
{
  tact_counter = tact;

  struct itimerval nval, oval;

  signal (TACT, IRC);

  nval.it_interval.tv_sec = 0;
  nval.it_interval.tv_usec = 500000;
  nval.it_value.tv_sec = 0;
  nval.it_value.tv_usec = 500000;

  /* Запускаем таймер */
  setitimer (ITIMER_REAL, &nval, &oval);

  int flag_ignore_tact_pulse = 0;

  sc_regGet (3, &flag_ignore_tact_pulse);
  while ((tact_counter >= 0 && flag_ignore_tact_pulse == 0) || 
  (tact_counter >= 0 && flag_not_ignore_tact != 0))
    {
      printTactCounter (tact_counter);
      pause ();
      sc_regGet (3, &flag_ignore_tact_pulse);
    }
  alarm (0);
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

/* Алгоритм работы одного такта устройства управления */
void
CU (int tact)
{
  int sign, command, operand;
  int accumulator, value_memory;
  int i, count_tact;
  if (tact)
    flag_not_ignore_tact = 1;
  else
    flag_not_ignore_tact = 0;

  while (1)
    {
      sc_icounterGet (&i);
      if (i < 0 || i >= SIZE_MEMORY)
        {
          sc_regSet (2, 1);
          break;
        }

      printCounters ();
      memoryController (i, &value_memory, GET);
      printCommand ();
      i++;

      if (sc_commandDecode (value_memory, &sign, &command, &operand)
          || sc_commandValidate (command) || sign == 1)
        {
          sc_regSet (4, 1);
          printCommand ();
          break;
        }
      clock_pulse_generator (0);
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
              return;
            }
          sc_accumulatorSet (value_memory);
          printAccumulator ();
          break;
        case STORE:
          sc_accumulatorGet (&accumulator);
          if (memoryController (operand, &accumulator, SET))
            {
              sc_regSet (2, 1);
              return;
            }
          printCell (operand, DEFAULT, DEFAULT);
          break;
        case ADD:
        case SUB:
        case DIVIDE:
        case MUL:
        case SUBC:
        case LOGLC:
          if (ALU (command, operand))
            return;
          break;
        case JUMP:
        case JNEG:
        case JZ:
          if (0 > operand || operand >= SIZE_MEMORY)
            {
              sc_regSet (2, 1);
              return;
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
              if (accumulator == 0 || accumulator == 0x4000)
                i = operand;
              break;
            }
          break;
        case HALT:
          return;
        default:
          break;
        }
      sc_icounterSet (i);
      if(tact)
      {
        count_tact++;
        if (count_tact >= tact)
          break;
      }
    }
}

int
ALU (int command, int operand)
{
  int old_accumulator, old_value_memory;
  int accumulator, value_memory;

  sc_accumulatorGet (&accumulator);
  old_accumulator = accumulator;
  if ((accumulator >> 14) == 1)
    {
      accumulator &= ~(1 << 14);
      accumulator *= -1;
    }

  if (memoryController (operand, &value_memory, GET))
    {
      sc_regSet (2, 1);
      return -1;
    }
  old_value_memory = value_memory;
  if ((value_memory >> 14) == 1)
    {
      value_memory &= ~(1 << 14);
      value_memory *= -1;
    }

  clock_pulse_generator (0);
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
      ALU (SUB, old_accumulator);
      return 0;
    case LOGLC:
      accumulator = value_memory << accumulator;
      accumulator &= ~(0 << 13);
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
  clock_pulse_generator (0);
  sc_accumulatorSet (accumulator);
  printAccumulator ();
  return 0;
}