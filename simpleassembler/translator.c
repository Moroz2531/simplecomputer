#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "string.h"
#include "translator.h"

#define SC_ENCODE(x, y, z) x << 14 | y << 7 | z

#define SIZE_MEMORY 128

int
command_encode (int sign, int command, int operand, int *value)
{
  if (1 < sign || sign < 0 || __INT8_MAX__ < command || command < 0)
    return -1;
  if (__INT8_MAX__ < operand || operand < 0 || value == NULL)
    return -1;
  *value = 0;
  *value |= SC_ENCODE (sign, command, operand);
  return 0;
}

static int
memory_save (int *memory, char *filename)
{
  FILE *file = fopen (filename, "wb");
  if (file == NULL)
    return -1;
  if (fwrite (memory, sizeof (int), SIZE_MEMORY, file) != SIZE_MEMORY)
    {
      fclose (file);
      return -1;
    }
  fclose (file);
  return 0;
}

static int
memory_load (int *memory, Var *var)
{
  while (var->next != NULL)
    var = var->next;

  int value;

  while (var != NULL)
    {
      switch (var->command)
        {
        case ASSIGMENT:
          if (var->operand_2 < 0)
            {
              var->operand_2 *= -1;
              var->operand_2 |= (1 << 14);
            }
          memory[var->operand_1] = var->operand_2;
          break;
        default:
          command_encode (0, var->command, var->operand_2, &value);
          memory[var->operand_1] = value;
        }
      var = var->prev;
    }
  return 0;
}

static char *
add_extension_for_file (char *src)
{
  size_t filename_size = strlen (src) + 1;
  char *dest = malloc (sizeof (char) * filename_size + 4);

  if (dest == NULL)
    return dest;

  strncpy (dest, src, filename_size);
  strncat (dest, ".o", 3);

  return dest;
}

static void
print_error (int err, int pos_string)
{
  switch (err)
    {
    case ERR_OPERAND:
      fprintf (stderr, "%d: operand is wrong\n", pos_string);
      break;
    case ERR_COMMAND:
      fprintf (stderr, "%d: command is wrong\n", pos_string);
      break;
    case ERR_CONST:
      fprintf (stderr, "%d: const is wrong\n", pos_string);
      break;
    case ERR_COUNT_VAR:
      fprintf (stderr, "%d: low/many variables\n", pos_string);
      break;
    case ERR_MEMORY:
      fprintf (stderr, "%d: error program. Memory \n", pos_string);
      break;
    case ERR_FILE:
      fprintf (stderr, "%d: file is not open/write\n", pos_string);
      break;
    default:
      fprintf (stderr, "%d: Error\n", pos_string);
      break;
    };
}

int
translator_simple_assembler (char *filename)
{
  FILE *file = fopen (filename, "r");
  if (file == NULL)
    {
      print_error (ERR_FILE, 0);
      return -1;
    }

  Var *var = NULL;
  String *string = string_create ();

  if (string == NULL)
    {
      print_error (ERR_MEMORY, 0);
      return -1;
    }

  short flag_comment = 0, flag_error = 0;
  int pos_string = 1;
  char ch;

  /* Обход всех строк, разбиение на токены и проверка синтаксиса */

  while (1)
    {
      ch = getc (file);
      if (ch == '\n' || ch == EOF)
        {
          if ((flag_error = string_parse (&var, string)))
            break;
          string_reload (string);
          pos_string++;
          flag_comment = 0;
          if (ch == EOF)
            break;
        }
      else if (ch == ';')
        flag_comment = 1;

      else if (flag_comment != 1)
        if ((flag_error = string_add (string, ch)))
          break;
    }
  if (ch != EOF)
    fclose (file);

  if (flag_error)
    {
      print_error (flag_error, pos_string);
      var_free (var);
      string_free (string);
      return -1;
    }

  /* Подготовка названия файла и загрузка операндов в двоичный файл */

  char *filename_write = add_extension_for_file (filename);
  if (filename_write == NULL)
    {
      print_error (ERR_MEMORY, pos_string);
      var_free (var);
      string_free (string);
      return -1;
    }
  int memory[SIZE_MEMORY] = { 0 };

  memory_load (memory, var);
  if (memory_save (memory, filename_write))
    {
      print_error (ERR_FILE, pos_string);
      var_free (var);
      string_free (string);
      free (filename_write);
      return -1;
    }
  var_free (var);
  string_free (string);
  free (filename_write);

  return 0;
}