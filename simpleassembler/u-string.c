#include <stdlib.h>
#include <string.h>

#include "u-string.h"

#define SIZE_MEMORY 128

enum {
  CAPACITY = 5,
  RESIZE_FACTOR = 2,
};

String *string_create() {
  String *str = malloc(sizeof(String));

  if (str != NULL) {
    str->capacity = CAPACITY;
    str->size = 0;
    str->string = malloc(sizeof(char) * str->capacity);
    if (str->string == NULL)
      free(str);
  }
  return str;
}

static int string_resize(String *str) {
  if (str == NULL)
    return -1;

  char capacity = str->capacity * RESIZE_FACTOR;
  char *string_new = malloc(sizeof(char) * capacity);
  if (string_new == NULL)
    return -1;

  if (strncpy(string_new, str->string, str->size) == NULL) {
    free(string_new);
    return -1;
  }
  free(str->string);

  str->string = string_new;
  str->capacity = capacity;

  return 0;
}

int string_add(String *str, char ch) {
  if (str == NULL)
    return -1;

  if (str->size == str->capacity)
    if (string_resize(str))
      return -1;
  str->string[str->size++] = ch;
  str->string[str->size] = '\0';

  return 0;
}

int string_reload(String *str) {
  if (str == NULL)
    return -1;

  str->size = 0;
  str->string[str->size] = '\0';
  if (str->capacity != CAPACITY) {
    str->string = realloc(str->string, CAPACITY);
    str->capacity = CAPACITY;
  }
  return 0;
}

void string_free(String *string) {
  if (string != NULL) {
    if (string->string != NULL)
      free(string->string);
    free(string);
  }
}

int string_parse(Var **var, String *str) {
  if (str == NULL)
    return -1;

  int value[3];
  int count_token = 0, flag_const = 0;

  const char delim[] = " \t";
  char *ptr = strtok(str->string, delim);

  for (int i = 0; ptr != NULL; i++, count_token++) {
    if (count_token >= 3)
      return ERR_COUNT_VAR;
    if (i % 2 == 0) {
      value[i] = atoi(ptr);
      if (flag_const) {
        if (value[i] < -16383 || value[i] > 16383)
          return ERR_CONST;
      } else if (value[i] < 0 || value[i] >= SIZE_MEMORY)
        return ERR_OPERAND;
    } else {
      value[i] = var_get_command(ptr);
      flag_const = (value[i] == ASSIGMENT ? 1 : 0);
      if (value[i] == UNKNOWN)
        return ERR_COMMAND;
    }
    ptr = strtok(NULL, delim);
  }
  if (count_token != 0 && count_token != 3)
    return ERR_COUNT_VAR;

  if (count_token == 3) {
    *var = var_add(var, value[0], value[2], value[1]);
    if (var == NULL)
      return ERR_MEMORY;
  }
  return 0;
}
