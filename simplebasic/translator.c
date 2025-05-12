#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "struct-string.h"
#include "var.h"

#include "translator.h"

#define SIZE_MEMORY 128

enum
{
  REM = 200,
  INPUT,
  OUTPUT,
  GOTO,
  IF,
  LET,
  END,
  UNKNOWN,

  NOP = 0,
  READ = 10,
  WRITE = 11,
  LOAD = 20,
  STORE = 21,
  ADD = 30,
  SUB = 31,
  DIVIDE = 32,
  MUL = 33,
  JUMP = 40,
  JNEG = 41,
  JZ = 42,
  HALT = 43,
};

#define PRINT_ERROR(msg, pos)                                                 \
  {                                                                           \
    print_error (msg, pos);                                                   \
    return -1;                                                                \
  }

/* вывод ошибки. 1 параметр - причина, 2 - позиция ошибки в строке */
static void
print_error (char *message, int pos_in_string)
{
  fprintf (stderr, "%d: %s\n", pos_in_string, message != NULL ? message : "");
}

/* добавление в имя файла расширение ".sa". Динамическое выделение памяти */
static char *
add_extension_for_file (char *src)
{
  size_t filename_size = strlen (src) + 1;
  char *dest = malloc (sizeof (char) * filename_size + 5);

  if (dest == NULL)
    return dest;

  strncpy (dest, src, filename_size);
  strncat (dest, ".sa", 4);

  return dest;
}

/* получение всех номеров строк. Строки сохраняются в Var */
/* первый проход по коду */
Var *
get_num_string (FILE *file)
{
  if (file == NULL)
    return NULL;

  String *str = string_create ();
  if (str == NULL)
    return NULL;
  Var *v = NULL, *v_main = NULL;

  for (char ch = getc (file);; ch = getc (file))
    {
      if (ch == '\n' || ch == EOF)
        {
          char *ptr = strtok (str->string, " \t");
          int num_string = atoi (ptr);

          v = var_add (&v, num_string);
          if (v->prev == NULL)
            v_main = v;
          if (v->prev != NULL && v->prev->num_string >= v->num_string)
            {
              print_error ("error number string", v->num_string);
              var_free (v);
              string_free (str);
              return NULL;
            }
          if (ch == EOF)
            break;
          string_reload (str);
          continue;
        }
      string_add (str, ch);
    }
  string_free (str);

  return v_main;
}

static int
simplebasic_get_code_command (char *src)
{
  if (strcmp (src, "REM") == 0)
    return REM;
  else if (strcmp (src, "INPUT") == 0)
    return INPUT;
  else if (strcmp (src, "OUTPUT") == 0)
    return OUTPUT;
  else if (strcmp (src, "GOTO") == 0)
    return GOTO;
  else if (strcmp (src, "IF") == 0)
    return IF;
  else if (strcmp (src, "LET") == 0)
    return LET;
  else if (strcmp (src, "END") == 0)
    return END;
  return UNKNOWN;
}

char *
simpleassembler_get_code_command (int code)
{
  char *buf = malloc (sizeof (char) * 8);
  switch (code)
    {
    case NOP:
      return (buf = strncpy (buf, "NOP", 4));
    case READ:
      return (buf = strncpy (buf, "READ", 5));
    case WRITE:
      return (buf = strncpy (buf, "WRITE", 6));
    case LOAD:
      return (buf = strncpy (buf, "LOAD", 5));
    case STORE:
      return (buf = strncpy (buf, "STORE", 6));
    case ADD:
      return (buf = strncpy (buf, "ADD", 4));
    case SUB:
      return (buf = strncpy (buf, "SUB", 4));
    case DIVIDE:
      return (buf = strncpy (buf, "DIVIDE", 7));
    case MUL:
      return (buf = strncpy (buf, "MUL", 4));
    case JUMP:
      return (buf = strncpy (buf, "JUMP", 5));
    case JNEG:
      return (buf = strncpy (buf, "JNEG", 5));
    case JZ:
      return (buf = strncpy (buf, "JZ", 3));
    case HALT:
      return (buf = strncpy (buf, "HALT", 5));
    }
  return NULL;
}

static int
file_load (FILE *file, Var *v)
{
  if (file == NULL || v == NULL)
    return -1;

  Var *v_temp = v;

  while (v_temp != NULL)
    {
      char *command = simpleassembler_get_code_command (v_temp->command);

      fprintf (file, "%d %s %d\n", v_temp->operand_1, command,
               v_temp->operand_2);
      free (command);
      v_temp = v_temp->prev;
    }

  return 0;
}

/* parse строки */
static int
string_parse (String *str, Var *src, Var **dest)
{
  if (str == NULL || src == NULL)
    return -1;

  /* адрес для simplecomputer */
  static int address_command = 0, address_operand = SIZE_MEMORY - 1;

  /* адреса переменных (исключает повторения в коде) */
  static void *var['Z' - 'A' + 1] = { NULL };

  const char delim[] = " \t\n";
  char *ptr = strtok (str->string, delim);

  ptr = strtok (NULL, delim);

  int command = simplebasic_get_code_command (ptr);
  int addr_operand;
  Var *v_temp = NULL;

  switch (command)
    {
    case REM:
      return 0;
    case INPUT:
    case OUTPUT:
      ptr = strtok (NULL, delim);
      if (strlen (ptr) > 1)
        return -1;
      if (var_check (ptr[0]))
        return -1;
      if (strtok (NULL, delim) != NULL)
        return -1;
      if (var[ptr[0] - 'A'] == NULL)
        {
          v_temp = var_add (dest, 0);
          var[ptr[0] - 'A'] = (void *)v_temp;
          addr_operand = address_operand--;
        }
      else
        {
          v_temp = var_add (dest, 0);
          addr_operand = ((Var *)var[ptr[0] - 'A'])->operand_2;
        }
      switch (command)
        {
        case INPUT:
          var_set (v_temp, address_command++, addr_operand, READ, ptr[0]);
          break;
        default:
          var_set (v_temp, address_command++, addr_operand, WRITE, ptr[0]);
        }
      break;
    case GOTO:
      /* НЕОБХОДИМО ЭТОТ БЛОК РЕАЛИЗОВАТЬ */
      break;
    case IF:
      /* НЕОБХОДИМО ЭТОТ БЛОК РЕАЛИЗОВАТЬ */
      break;
    case LET:
      /* НЕОБХОДИМО ЭТОТ БЛОК РЕАЛИЗОВАТЬ */
      break;
    case END:
      if (strtok (NULL, delim) != NULL)
        return -1;
      v_temp = var_add (dest, 0);
      var_set (v_temp, address_command++, 0, HALT, '0');
      break;
    default:
      return -1;
    };
  *dest = v_temp;
  return 0;
}

/* транслятор с simplebasic в simpleassembler */
int
translator_simple_basic (char *filename)
{
  /* открытие необходимых файлов, создание переменных и их проверка */

  FILE *file = fopen (filename, "r");
  if (file == NULL)
    PRINT_ERROR ("file is not open", 0)

  String *str = string_create ();
  if (str == NULL)
    {
      fclose (file);
      PRINT_ERROR ("string is not create", 0)
    }

  Var *v = get_num_string (file), *v_write = NULL;
  if (v == NULL)
    {
      fclose (file);
      string_free (str);
      PRINT_ERROR ("var is not create", 0)
    }
  file = fopen (filename, "r");

  /* записываем всю строку в str */
  char ch = getc (file);
  for (;; ch = getc (file))
    {
      if (ch == '\n' || ch == EOF)
        {
          if (string_parse (str, v, &v_write))
            {
              var_free (v);
              var_free (v_write);
              string_free (str);
              fclose (file);
              PRINT_ERROR ("Error in line", v->num_string)
            }
          if (v->next != NULL)
            v = v->next;
          string_reload (str);
          if (ch == EOF)
            break;
        }
      string_add (str, ch);
    }

  /* создание файла и запись в файл */

  char *filename_for_write = add_extension_for_file (filename);
  FILE *file_write = fopen (filename_for_write, "w");

  if (file_write != NULL)
    file_load (file_write, v_write);
  free (filename_for_write);
  var_free (v);
  var_free (v_write);
  string_free (str);
  fclose (file_write);

  return 0;
}