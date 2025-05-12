#include <stdio.h>

#include "translator.h"

int
main (int argv, char *argc[])
{
  if (argv == 1)
    {
      printf ("Enter input file in argument\n");
      return -1;
    }
  if (argv > 2)
    {
      printf ("Many arguments entered\n");
      return -1;
    }
  translator_simple_basic (argc[1]);
  return 0;
}