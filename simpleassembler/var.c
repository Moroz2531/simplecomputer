#include <stdlib.h>

#include "var.h"

static void
var_init (Var *v)
{
  v->pos_string = 0;
  v->length_1 = 0;
  v->length_2 = 0;
  v->length_command = 0;

  v->prev = NULL;
  v->next = NULL;
}

Var *
var_create (Var *v)
{
  Var *new_v = malloc (sizeof (Var));
  var_init (new_v);

  if (v != NULL)
    {
      while (v->next != NULL)
        v = v->next;
      v->next = new_v;
      new_v->prev = v;
    }
  return new_v;
}

void
var_free (Var *v)
{
  Var *v_temp1_1 = v->prev, *v_temp2_1;
  Var *v_temp1_2 = v->next, *v_temp2_2;

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

int
var_add (Var *v, int var, char ch)
{
  switch (var)
    {
    case OPERAND_1:
    case OPERAND_2:
      if (v->length_2 >= MAX_LENGTH_OPERAND)
        return -1;
      switch (var)
        {
        case OPERAND_1:
          v->operand_1[v->length_1++] = ch;
          v->operand_1[v->length_1] = '\0';
          break;

        default:
          v->operand_2[v->length_2++] = ch;
          v->operand_2[v->length_2] = '\0';
        }
      break;

    case COMMAND:
      if (v->length_command >= MAX_LENGTH_COMMAND)
        return -1;
      v->command[v->length_command++] = ch;
      v->command[v->length_command] = '\0';
      break;

    default:
      return -1;
    };

  return 0;
}