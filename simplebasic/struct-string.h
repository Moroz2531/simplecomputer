#ifndef STRING_H
#define STRING_H

typedef struct String
{
  char *string;
  unsigned int size, capacity;
} String;

String *string_create ();

int string_add (String *str, char ch);
int string_reload (String *str);

void string_free (String *string);

#endif