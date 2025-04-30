#include <stdio.h>

#include "translator.h"

#define MAX_LENGTH_COMMAND 8 // in byte
#define MAX_LENGTH_OPERAND 4
#define SIZE_MEMORY 128

int memory[SIZE_MEMORY];

enum command
{
  NOP = 0,
  CPUINFO = 1,
  READ = 10,
  WRITE = 11,
  LOAD = 20,
  STORE = 21,
  ADD = 30,
  SUB = 31,
  DIVIDE = 32,
  MUL = 33,
  JUMP = 40,
  JNEG = 41,
  JZ = 42,
  HALT = 43,
  SUBC = 66,
  LOGLC = 67,
};

int
translator_simple_assembler (char *filename)
{
  FILE *file = fopen (filename, "r");
  if (file == NULL)
    {
      perror ("");
      return -1;
    }

  short flags_str = 0, shift = 5;
  char operand[2][MAX_LENGTH_OPERAND], command[MAX_LENGTH_COMMAND];
  char ch;

  /* for statistics (output errors) */
  int count_string = 0, pos_ch = 0;

  for (char ch; (ch = getc (file)) != EOF; pos_ch++)
    {
      if (ch == '\n')
        {
          // write operands and command
          flags_str = 0;
          shift = 5;
          pos_ch = 0;
          count_string++;
          continue;
        }
      if (ch == ' ' || ch == '\t' || (flags_str & 1) == 1)
        {
          if (flags_str != 0)
            flags_str |= (1 << shift);
          continue;
        }
      if (('0' <= ch || ch <= '9') && (flags_str == 0 || flags_str == 0x3C))
        {
          shift--;
        }
    }

  fclose (file);

  return 0;
}