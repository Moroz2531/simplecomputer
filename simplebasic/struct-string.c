#include <stdlib.h>
#include <string.h>

#include "struct-string.h"

enum
{
  CAPACITY = 64, // начальный размер строки в байтах
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