#ifndef VAR_H
#define VAR_H

#define MAX_LENGTH_COMMAND 8 // in byte
#define MAX_LENGTH_OPERAND 4

enum
{
  OPERAND_1,
  OPERAND_2,
  COMMAND
};

typedef struct Var
{
  int pos_string;
  int length_1, length_2, length_command;
  char operand_1[MAX_LENGTH_OPERAND];
  char operand_2[MAX_LENGTH_OPERAND];
  char command[MAX_LENGTH_COMMAND];

  struct Var *next;
  struct Var *prev;
} Var;

void var_free (Var *v);
Var *var_create (Var *v);
int var_add (Var *v, int var, char ch);

#endif