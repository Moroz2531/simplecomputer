#ifndef STRING_H
#define STRING_H

#include "var.h"

typedef struct String
{
  char *string;
  unsigned int size, capacity;
} String;

enum errors
{
  ERR_OPERAND = 1,
  ERR_COMMAND,
  ERR_CONST,
  ERR_COUNT_VAR,
  ERR_MEMORY,
  ERR_FILE
};

String *string_create ();

int string_parse (Var **var, String *str);
int string_add (String *str, char ch);
int string_reload (String *str);

void string_free (String *string);

#endif