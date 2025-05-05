#ifndef VAR_H
#define VAR_H

#define MAX_LENGTH_COMMAND 8 // in byte
#define MAX_LENGTH_OPERAND 4

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
  ASSIGMENT = 128,
  UNKNOWN = 129
};

typedef struct Var
{
  int operand_1, operand_2;
  int command;

  struct Var *next;
  struct Var *prev;
} Var;

Var *var_add (Var **v, int operand_1, int operand_2, int command);

int var_get_command (char *command);

void var_free (Var *v);

#endif