#ifndef VAR_H
#define VAR_H

typedef struct Var
{
  char var_name;
  int num_string;
  int operand_1, operand_2, command;

  struct Var *next;
  struct Var *prev;

} Var;

Var *var_add (Var **v, int num_string);

int var_set (Var *v, int operand_1, int operand_2, int command, char var_name);
int var_check (char ch);
int var_del (Var **v);

void var_free (Var *v);

#endif