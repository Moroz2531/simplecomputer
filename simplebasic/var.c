#include <stdlib.h>

#include "var.h"

static Var *
var_create (int num, int operand_1, int operand_2, int command)
{
  Var *v = malloc (sizeof (Var));
  if (v != NULL)
    {
      v->num_string = num;
      v->operand_1 = operand_1;
      v->operand_2 = operand_2;
      v->command = command;

      v->next = NULL;
      v->prev = NULL;
    }
  return v;
}

Var *
var_add (Var **v, int num_string)
{
  Var *v_new = var_create (num_string, 0, 0, 0);
  if (v != NULL && *v != NULL)
    {
      while ((*v)->next != NULL)
        *v = (*v)->next;
      (*v)->next = v_new;
      v_new->prev = *v;
    }
  return v_new;
}

int
var_set (Var *v, int operand_1, int operand_2, int command, char var_name)
{
  v->operand_1 = operand_1;
  v->operand_2 = operand_2;
  v->command = command;
  v->var_name = var_name;
  return 0;
}

int
var_del (Var **v)
{
  if (v != NULL && *v != NULL)
    {
      Var *v_prev = (*v)->prev, *v_next = (*v)->next;
      free (*v);
      if (v_prev != NULL)
        v_prev->next = v_next;
      if (v_next != NULL)
        v_next->prev = v_prev;
    }
  return 0;
}

int
var_check (char ch)
{
  if ('A' <= ch && ch <= 'Z')
    return 0;
  return -1;
}

void
var_free (Var *v)
{
  if (v == NULL)
    return;

  Var *v_temp1_1 = v->prev, *v_temp2_1;
  Var *v_temp1_2 = v->next, *v_temp2_2;

  free (v);

  while (v_temp1_1 != NULL)
    {
      v_temp2_1 = v_temp1_1->prev;
      free (v_temp1_1);
      v_temp1_1 = v_temp2_1;
    }
  while (v_temp1_2 != NULL)
    {
      v_temp2_2 = v_temp1_2->next;
      free (v_temp1_2);
      v_temp1_2 = v_temp2_2;
    }
}