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

/* Получение всех строк из файла */
String_array *
get_string_array (FILE *file)
{
  if (file == NULL)
    return NULL;

  String_array *str = string_array_add (NULL, string_create ());
  String_array *str_temp = str;
  if (str == NULL)
    return NULL;

  for (char ch = getc (file);; ch = getc (file))
    {
      if (ch == '\n' || ch == EOF)
        {
          if (str_temp->str->size != 0)
            {
              int index = atoi (str_temp->str->string);
              if (str_temp->prev != NULL && str_temp->prev->index >= index)
                {
                  print_error ("error in number string", index);
                  string_array_free (str);
                  return NULL;
                }
              str_temp->index = index;

              str_temp = string_array_add (str_temp, string_create ());
              if (str_temp == NULL)
                {
                  print_error ("string array - not malloc", -1);
                  string_array_free (str);
                  return NULL;
                }
            }
          if (ch == EOF)
            break;
        }
      else
        string_add (str_temp->str, ch);
    }
  return str;
}

static int
simplebasic_get_code_command (char *src)
{
  if (src != NULL)
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
    }
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
file_load (FILE *file, String_array *str)
{
  if (file == NULL || str == NULL)
    return -1;

  for (char *command; str != NULL; str = str->next)
    {
      if (str->str_is_read == 0)
        continue;

      for (Var *token = str->token; token != NULL; token = token->next)
        {
          command = simpleassembler_get_code_command (token->command);
          if (command == NULL)
            return -1;
          fprintf (file, "%d %s %d\n", token->operand_1, command,
                   token->operand_2);
        }
    }
  return 0;
}

/* parse строки */
static int
string_parse (String_array **s_arr)
{
  String_array *str_arr = *s_arr, *str_arr_temp = str_arr;
  String *str = str_arr->str;
  if (str_arr == NULL || str_arr->str == NULL || str_arr->str->string == NULL)
    {
      print_error ("str_arr is null", -1);
      return -1;
    }
  if (str_arr->str_is_read)
    return 0;
  str_arr->str_is_read = 1;

  static Var *ptr_token_for_goto = NULL;

  const char delim[] = " \t\n";
  char *ptr;
  if ((ptr = strtok (str->string, delim)) == NULL)
    return 0;

  /* адреса для simplecomputer */
  static int address_command = 0, address_operand = SIZE_MEMORY - 1;
  if (address_command >= address_operand || address_operand <= address_command)
    {
      print_error ("segmentation fault", str_arr->index);
      return -1;
    }
  /* адреса переменных (исключает повторения переменных в коде) */
  static void *var['Z' - 'A' + 1] = { NULL };

  ptr = strtok (NULL, delim);

  int command = simplebasic_get_code_command (ptr), addr_operand, value;

  if (command == UNKNOWN)
    return -1;

  switch (command)
    {
    case REM:
      if (ptr_token_for_goto != NULL)
        {
          print_error ("goto is wrong", str_arr->index);
          return -1;
        }
      return 0;
    case INPUT:
    case OUTPUT:
      ptr = strtok (NULL, delim);
      if (strlen (ptr) > 1)
        return -1;
      if (var_check (ptr[0]))
        return -1;
      switch (command)
        {
        case INPUT:
          if (var[ptr[0] - 'A'] == NULL)
            addr_operand = address_operand--;
          else
            addr_operand = ((Var *)var[ptr[0] - 'A'])->operand_2;

          var_add (&str_arr->token, address_command++, addr_operand, READ);
          var[ptr[0] - 'A'] = (void *)str_arr->token;
          break;
        default:
          if (var[ptr[0] - 'A'] == NULL)
            return -1;
          addr_operand = ((Var *)var[ptr[0] - 'A'])->operand_2;
          var_add (&str_arr->token, address_command++, addr_operand, WRITE);
        }
      break;
    case GOTO:
      ptr = strtok (NULL, delim);
      value = atoi (ptr);
      str_arr_temp = string_array_get_string_index (str_arr, value);
      if (str_arr_temp == NULL)
        return -1;
      if (str_arr_temp->str_is_read)
        {
          if (str_arr_temp != str_arr)
            {
              if (str_arr_temp->token == NULL)
                {
                  print_error ("goto is wrong", str_arr_temp->index);
                  return -1;
                }
              var_add (&str_arr->token, address_command++,
                       str_arr_temp->token->operand_1, JUMP);
            }
          else
            {
              address_command++;
              var_add (&str_arr->token, address_command, address_command,
                       JUMP);
            }
          if (ptr_token_for_goto != NULL)
            {
              ptr_token_for_goto->operand_2 = str_arr->token->operand_1;
              ptr_token_for_goto = NULL;
            }
        }
      else
        {
          var_add (&str_arr->token, address_command++, -1, JUMP);
          if (ptr_token_for_goto != NULL)
            ptr_token_for_goto->operand_2 = str_arr->token->operand_1;
          ptr_token_for_goto = str_arr->token;
        }
      if (strtok (NULL, delim) != NULL)
        return -1;
      *s_arr = str_arr_temp;
      return 0;
    case IF:
      /* НЕОБХОДИМО ЭТОТ БЛОК РЕАЛИЗОВАТЬ */
      break;
    case LET:
      /* НЕОБХОДИМО ЭТОТ БЛОК РЕАЛИЗОВАТЬ */
      break;
    case END:
      var_add (&str_arr->token, address_command++, 0, HALT);
      break;
    default:
      return -1;
    };
  if (strtok (NULL, delim) != NULL)
    return -1;
  if (ptr_token_for_goto != NULL)
    {
      ptr_token_for_goto->operand_2 = str_arr->token->operand_1;
      ptr_token_for_goto = NULL;
    }
  return 0;
}

/* транслятор с simplebasic в simpleassembler */
int
translator_simple_basic (char *filename)
{
  /* открытие необходимых файлов, создание переменных и их проверка */

  FILE *file = fopen (filename, "r");
  if (file == NULL)
    PRINT_ERROR ("file is not open", -1)

  String_array *str = get_string_array (file), *str_temp = str, *cur = NULL;
  if (str == NULL)
    {
      if (file != NULL)
        fclose (file);
      return -1;
    }
  while (str_temp != NULL)
    {
      cur = str_temp;
      if (string_parse (&str_temp))
        {
          print_error ("error in line", str_temp->index);
          string_array_free (str);
          return -1;
        }
      if (cur != str_temp)
        continue;
      str_temp = str_temp->next;
    }

  /* создание файла и запись в файл */

  char *filename_for_write = add_extension_for_file (filename);
  FILE *file_write = fopen (filename_for_write, "w");

  if (file_write != NULL)
    {
      if (file_load (file_write, str))
        print_error ("file is not load", -1);
      fclose (file_write);
    }
  if (filename_for_write != NULL)
    free (filename_for_write);
  string_array_free (str);

  return 0;
}