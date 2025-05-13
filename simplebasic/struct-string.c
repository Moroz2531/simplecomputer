#include <stdlib.h>
#include <string.h>

#include "struct-string.h"

enum
{
  CAPACITY = 32, // начальный размер строки в байтах
  RESIZE_FACTOR = 2, // множитель увеличения размера строки
};

/* создание строки */
String *
string_create ()
{
  String *str = malloc (sizeof (String));

  if (str != NULL)
    {
      str->capacity = CAPACITY;
      str->size = 0;
      str->string = malloc (sizeof (char) * str->capacity);
      if (str->string == NULL)
        free (str);
    }
  return str;
}

String_array *
string_array_create (String *str)
{
  String_array *str_arr = malloc (sizeof (String_array));
  static int index = 1;
  if (str_arr != NULL)
    {
      str_arr->str = str;
      str_arr->str_is_read = 0;
      str_arr->index = index++;
      str_arr->prev = NULL;
      str_arr->next = NULL;
    }
  return str_arr;
}

String_array *
string_array_add (String_array *str_arr, String *str)
{
  if (str == NULL)
    return NULL;

  String_array *new = string_array_create (str);
  if (str_arr != NULL)
    {
      while (str_arr->next != NULL)
        str_arr = str_arr->next;
      str_arr->next = new;
      if (new != NULL)
        new->prev = str_arr;
    }
  return new;
}

String_array *
string_array_get_string_index (String_array *arr, int index)
{
  if (arr != NULL)
    {
      if (arr->index < index)
        while (arr != NULL && arr->index < index)
          {
            if (arr->index == index)
              return arr;
            arr = arr->next;
          }
      else if (arr->index > index)
        while (arr != NULL && arr->index > index)
          {
            if (arr->index == index)
              return arr;
            arr = arr->prev;
          }
      else
        return arr;
    }
  return arr;
}

int
string_array_set_string_is_read (String_array *str)
{
  if (str == NULL)
    return -1;
  str->str_is_read = 1;
  return 0;
}

void
string_array_free (String_array *str_arr)
{
  if (str_arr == NULL)
    return;
  String_array *prev = str_arr->prev, *next = str_arr->next, *temp = NULL;

  string_free (str_arr->str);
  free (str_arr);

  while (prev != NULL)
    {
      temp = prev->prev;
      string_free (prev->str);
      free (prev);
      prev = temp;
    }
  while (next != NULL)
    {
      temp = next->next;
      string_free (next->str);
      free (next);
      next = temp;
    }
}

/* увеличение размера строки в RESIZE_FACTOR */
static int
string_resize (String *str)
{
  if (str == NULL)
    return -1;

  char capacity = str->capacity * RESIZE_FACTOR;
  char *string_new = malloc (sizeof (char) * capacity);
  if (string_new == NULL)
    return -1;

  if (strncpy (string_new, str->string, str->size) == NULL)
    {
      free (string_new);
      return -1;
    }
  free (str->string);

  str->string = string_new;
  str->capacity = capacity;

  return 0;
}

/* добавление элемента (типа char) в строку */
int
string_add (String *str, char ch)
{
  if (str == NULL)
    return -1;

  if (str->size == str->capacity)
    if (string_resize (str))
      return -1;
  str->string[str->size++] = ch;
  str->string[str->size] = '\0';

  return 0;
}

/* возвращение строки к изначальному состоянию */
int
string_reload (String *str)
{
  if (str == NULL)
    return -1;

  str->size = 0;
  str->string[str->size] = '\0';
  if (str->capacity != CAPACITY)
    {
      str->string = realloc (str->string, CAPACITY);
      str->capacity = CAPACITY;
    }
  return 0;
}

/* освобождение строки */
void
string_free (String *string)
{
  if (string != NULL)
    {
      if (string->string != NULL)
        free (string->string);
      free (string);
    }
}