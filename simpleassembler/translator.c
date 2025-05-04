#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "translator.h"
#include "var.h"

#define SC_ENCODE(x, y, z) x << 14 | y << 7 | z

#define SIZE_MEMORY 128

enum command
{
  NOP = 0,
  CPUINFO = 1,
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
  SUBC = 66,
  LOGLC = 67,
  ASSIGMENT = 128,
};

static int
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
  while (var->prev != NULL)
    var = var->prev;

  for (int operand[2], command, value; var != NULL; var = var->next)
    {
      if (var->length_1 == 0)
        continue;

      operand[0] = atoi (var->operand_1);
      operand[1] = atoi (var->operand_2);

      if (strcmp (var->command, "NOP"))
        command = NOP;
      else if (strcmp (var->command, "CPUINFO"))
        command = CPUINFO;
      else if (strcmp (var->command, "READ"))
        command = READ;
      else if (strcmp (var->command, "WRITE"))
        command = WRITE;
      else if (strcmp (var->command, "LOAD"))
        command = LOAD;
      else if (strcmp (var->command, "STORE"))
        command = STORE;
      else if (strcmp (var->command, "ADD"))
        command = ADD;
      else if (strcmp (var->command, "SUB"))
        command = SUB;
      else if (strcmp (var->command, "DIVIDE"))
        command = DIVIDE;
      else if (strcmp (var->command, "MUL"))
        command = MUL;
      else if (strcmp (var->command, "JUMP"))
        command = JUMP;
      else if (strcmp (var->command, "JNEG"))
        command = JNEG;
      else if (strcmp (var->command, "JZ"))
        command = JZ;
      else if (strcmp (var->command, "HALT"))
        command = HALT;
      else if (strcmp (var->command, "SUBC"))
        command = SUBC;
      else if (strcmp (var->command, "LOGLC"))
        command = LOGLC;
      else if (strcmp (var->command, "="))
        command = ASSIGMENT;
      else
        return var->pos_string;

      if (operand[0] < 0 || operand[0] >= SIZE_MEMORY)
        return var->pos_string;

      if (command == ASSIGMENT)
        {
          if (operand[1] < 0)
            {
              operand[1] *= -1;
              operand[1] |= (1 << 14);
            }
          if (operand[1] > __SHRT_MAX__)
            return var->pos_string;
          memory[operand[0]] = operand[1];
        }
      else
        {
          if (command_encode (0, command, operand[1], &value))
            return var->pos_string;
          memory[operand[0]] = value;
        }
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

int
flags_set (short *flags, int pos)
{
  *flags |= (1 << pos);
  return 0;
}

int
flags_get (short flags, int pos)
{
  return (flags >> pos) & 1;
}

#define SET_FLAGS_AND_INCREASE_SHIFT                                          \
  flags_set (&flags_str, shift);                                              \
  flags_set (&flags_str, shift_read);                                         \
  shift--;

int
translator_simple_assembler (char *filename)
{
  FILE *file = fopen (filename, "r");
  if (file == NULL)
    {
      fprintf (stderr, "File for reading does not open\n");
      return -1;
    }

  short flags_str = 0, shift = 5;
  const short shift_err = 8, shift_read = 9, shift_assigment = 10;

  Var *var = var_create (NULL), *var_main = var;
  if (var == NULL)
    {
      fprintf (stderr, "New var doesn`t create\n");
      return -1;
    }

  /* for statistics (output errors) */
  int pos_string = 1, pos_ch = 1;

  char ch;
  for (; (ch = getc (file)) != EOF; pos_ch++)
    {
      if (flags_get (flags_str, shift_read))
        {
          const short flags_main = flags_str & 0x7F;

          if (ch != ' ' || ch != '\t')
            {
              switch (flags_main)
                {
                case 0x20:
                  if (var_add (var, OPERAND_1, ch))
                    flags_str |= (1 << shift_err);
                  break;
                case 0x38:
                  if (var_add (var, OPERAND_2, ch))
                    flags_str |= (1 << shift_err);
                  break;
                default:
                  if (var_add (var, COMMAND, ch))
                    flags_str |= (1 << shift_err);
                  break;
                }
              if ((flags_str >> shift_err) & 1)
                break;
              continue;
            }
          else
            flags_str = ~(1 << shift_read);
        }
      if (flags_get (flags_str, shift_assigment))
        {
          if (ch != ' ' || ch != '\t')
            {
              if (ch < '0' || ch > '9')
                {
                  flags_set (&flags_str, shift_err);
                  break;
                }
              var_add (var, OPERAND_2, ch);
              continue;
            }
          else
            flags_str = ~(1 << shift_assigment);
        }

      if (ch == '\n')
        {
          printf ("1. %p\n", var);
          var->pos_string = pos_string;
          var = var_create (var_main);
          printf ("2. %p\n", var);
          if (var == NULL)
            {
              fprintf (stderr, "New var doesn`t create\n");
              var_free (var_main);
              return -1;
            }
          flags_str = 0;
          shift = 5;
          pos_ch = 0;
          pos_string++;
          continue;
        }
      if (flags_get (flags_str, 0))
        continue;

      if (ch == ' ' || ch == '\t')
        {
          if (flags_str != 0 && flags_get (flags_str, 1) != 1)
            flags_str |= (1 << shift);
          continue;
        }
      if ((flags_str == 0 || flags_get (flags_str, 2))
          && ('0' <= ch && ch <= '9'))
        {
          SET_FLAGS_AND_INCREASE_SHIFT
          if (flags_str == 0)
            var_add (var, OPERAND_1, ch);
          else
            var_add (var, OPERAND_2, ch);
        }
      else if ((flags_get (flags_str, 4))
               && (('A' <= ch && ch <= 'Z') || ch == '='))
        {
          if (ch == '=')
            {
              var_add (var, COMMAND, ch);
              flags_set (&flags_str, shift_assigment);
              shift--;
              continue;
            }
          SET_FLAGS_AND_INCREASE_SHIFT
          var_add (var, COMMAND, ch);
        }
      else if ((flags_str == 0 || flags_get (flags_str, 1)) && ch == ';')
        {
          flags_set (&flags_str, 0);
        }
      else
        {
          flags_set (&flags_str, shift_err);
          break;
        }
    }
  if (ch != EOF)
    fclose (file);

  printf ("1\n");

  if (flags_get (flags_str, shift_err))
    {
      fprintf (stderr, "Error in line: %d; pos: %d\n", pos_string, pos_ch);
      var_free (var_main);
      return -1;
    }

  printf ("2\n");

  char *filename_write = add_extension_for_file (filename);
  if (filename_write == NULL)
    {
      fprintf (stderr, "In file name not add extension\n");
      var_free (var_main);
      return -1;
    }
  int memory[SIZE_MEMORY], pos_string_err;

  printf ("3\n");

  if ((pos_string_err = memory_load (memory, var_main)))
    {
      fprintf (stderr, "Error in line: %d\n", pos_string_err);
      var_free (var_main);
      free (filename_write);
      return -1;
    }

  printf ("4\n");
  if (memory_save (memory, filename_write))
    {
      fprintf (stderr, "File for writing does not open/write\n");
      var_free (var_main);
      free (filename_write);
      return -1;
    }

  printf ("5\n");

  var_free (var_main);
  free (filename_write);

  printf ("6\n");

  return 0;
}