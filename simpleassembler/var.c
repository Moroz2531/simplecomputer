#include <stdlib.h>
#include <string.h>

#include "var.h"

static Var *var_create(int operand_1, int operand_2, int command) {
  Var *v = malloc(sizeof(Var));
  if (v != NULL) {
    v->operand_1 = operand_1;
    v->operand_2 = operand_2;
    v->command = command;

    v->next = NULL;
    v->prev = NULL;
  }
  return v;
}

Var *var_add(Var **v, int operand_1, int operand_2, int command) {
  Var *v_new = var_create(operand_1, operand_2, command);
  if (*v != NULL) {
    while ((*v)->next != NULL)
      *v = (*v)->next;
    (*v)->next = v_new;
    v_new->prev = *v;
  }
  return v_new;
}

void var_free(Var *v) {
  if (v == NULL)
    return;

  Var *v_temp1_1 = v->prev, *v_temp2_1;
  Var *v_temp1_2 = v->next, *v_temp2_2;

  free(v);

  while (v_temp1_1 != NULL) {
    v_temp2_1 = v_temp1_1->prev;
    free(v_temp1_1);
    v_temp1_1 = v_temp2_1;
  }
  while (v_temp1_2 != NULL) {
    v_temp2_2 = v_temp1_2->next;
    free(v_temp1_2);
    v_temp1_2 = v_temp2_2;
  }
}

int var_get_command(char *command) {
  if (strcmp(command, "NOP") == 0)
    return NOP;
  else if (strcmp(command, "CPUINFO") == 0)
    return CPUINFO;
  else if (strcmp(command, "READ") == 0)
    return READ;
  else if (strcmp(command, "WRITE") == 0)
    return WRITE;
  else if (strcmp(command, "LOAD") == 0)
    return LOAD;
  else if (strcmp(command, "STORE") == 0)
    return STORE;
  else if (strcmp(command, "ADD") == 0)
    return ADD;
  else if (strcmp(command, "SUB") == 0)
    return SUB;
  else if (strcmp(command, "DIVIDE") == 0)
    return DIVIDE;
  else if (strcmp(command, "MUL") == 0)
    return MUL;
  else if (strcmp(command, "JUMP") == 0)
    return JUMP;
  else if (strcmp(command, "JNEG") == 0)
    return JNEG;
  else if (strcmp(command, "JZ") == 0)
    return JZ;
  else if (strcmp(command, "HALT") == 0)
    return HALT;
  else if (strcmp(command, "SUBC") == 0)
    return SUBC;
  else if (strcmp(command, "LOGLC") == 0)
    return LOGLC;
  else if (strcmp(command, "=") == 0)
    return ASSIGMENT;
  else
    return UNKNOWN;
}
