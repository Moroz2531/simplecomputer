#ifndef STRING_H
#define STRING_H

typedef struct String
{
  char *string;
  unsigned int size, capacity;
} String;

typedef struct String_array
{
  String *str;
  int str_is_read, index;

  struct Var *token;

  struct String_array *prev;
  struct String_array *next;
} String_array;

String_array *string_array_create (String *str);
String_array *string_array_add (String_array *str_arr, String *str);
String_array *string_array_get_string_index (String_array *arr, int index);

String *string_create ();

int string_array_set_string_is_read (String_array *str);
int string_add (String *str, char ch);
int string_reload (String *str);

void string_array_free (String_array *str_arr);
void string_free (String *string);

#endif