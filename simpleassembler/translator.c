#include <stdio.h>

#include "translator.h"

int
translator_simple_assembler (char *filename)
{
  FILE *file = fopen (filename, "r");
  if (file == NULL)
    {
      perror ("");
      return -1;
    }

  char ch;
  while ((ch = getc (file)) != EOF)
    {
      printf ("%c", ch);
    }
  printf ("\n");

  fclose (file);

  return 0;
}