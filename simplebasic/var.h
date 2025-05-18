#ifndef VAR_H
#define VAR_H

typedef struct Var
{
  int operand_1, operand_2, command;

  struct Var *next;
  struct Var *prev;

} Var;

Var *var_add (Var **v, int op_1, int op_2, int command);

int var_set (Var *v, int op_1, int op_2, int command);
int var_check (char ch);
int var_del (Var **v);

void var_free (Var *v);

#endif