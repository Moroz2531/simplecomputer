#include <fcntl.h>
#include <unistd.h>

#include "myBigChars.h"

#define SET_DIAGONALS(big, x, y, bit)                                         \
  {                                                                           \
    bc_setbigcharpos (big, y, x, bit);                                        \
    bc_setbigcharpos (big, x, y, bit);                                        \
  }

#define SET_PARALLELS(big, x, y, bit)                                         \
  {                                                                           \
    SET_DIAGONALS (big, x, y, bit);                                           \
    bc_setbigcharpos (big, x, x, bit);                                        \
    bc_setbigcharpos (big, y, y, bit);                                        \
  }

#define SET_FULL_LINE_0_1                                                     \
  for (int i = 0; i <= 7; i++)                                                \
    {                                                                         \
      bc_setbigcharpos (big, 0, i, 1);                                        \
      bc_setbigcharpos (big, 1, i, 1);                                        \
    }

#define SET_FULL_LINE_3_4                                                     \
  for (int i = 0; i <= 7; i++)                                                \
    {                                                                         \
      bc_setbigcharpos (big, 3, i, 1);                                        \
      bc_setbigcharpos (big, 4, i, 1);                                        \
    }

#define SET_FULL_LINE_6_7                                                     \
  for (int i = 0; i <= 7; i++)                                                \
    {                                                                         \
      bc_setbigcharpos (big, 6, i, 1);                                        \
      bc_setbigcharpos (big, 7, i, 1);                                        \
    }

#define SET_FULL_LINE_0_1_3_4_6_7                                             \
  SET_FULL_LINE_0_1                                                           \
  SET_FULL_LINE_3_4                                                           \
  SET_FULL_LINE_6_7

static int
generate_big_zero (int *big)
{
  for (int i = 2; i <= 5; i++)
    {
      bc_setbigcharpos (big, 0, i, 1);
      bc_setbigcharpos (big, 7, i, 1);
    }
  for (int i = 1; i <= 5; i++)
    {
      bc_setbigcharpos (big, i, 0, 1);
      bc_setbigcharpos (big, i, 7, 1);
    }
  for (int i = 3; i <= 4; i++)
    {
      SET_DIAGONALS (big, 2, i, 1)
      SET_DIAGONALS (big, 5, i, 1)
    }
  SET_PARALLELS (big, 1, 6, 1)

  return 0;
}

static int
generate_big_one (int *big)
{
  for (int i = 0; i <= 7; i++)
    {
      bc_setbigcharpos (big, i, 4, 1);
      bc_setbigcharpos (big, i, 5, 1);
    }
  for (int i = 1; i <= 3; i++)
    bc_setbigcharpos (big, i, (4 - i), 1);

  SET_DIAGONALS (big, 2, 3, 1)

  return 0;
}

static int
generate_big_two (int *big)
{
  SET_FULL_LINE_0_1_3_4_6_7

  for (int i = 0; i <= 1; i++)
    {
      bc_setbigcharpos (big, 2, 6 + i, 1);
      bc_setbigcharpos (big, 5, 0 + i, 1);
    }
  SET_PARALLELS (big, 0, 7, 0)

  return 0;
}

static int
generate_big_three (int *big)
{
  for (int i = 2; i <= 5; i++)
    {
      bc_setbigcharpos (big, 0, i, 1);
      bc_setbigcharpos (big, 1, i, 1);
    }
  for (int i = 4; i <= 7; i++)
    {
      bc_setbigcharpos (big, 3, i, 1);
      bc_setbigcharpos (big, 4, i, 1);
    }
  for (int i = 0; i <= 1; i++)
    {
      bc_setbigcharpos (big, 2, i, 1);
      bc_setbigcharpos (big, 5, i, 1);
    }
  SET_PARALLELS (big, 1, 6, 1)

  return 0;
}

static int
generate_big_four (int *big)
{
  for (int i = 0; i <= 7; i++)
    {
      bc_setbigcharpos (big, i, 6, 1);
      bc_setbigcharpos (big, i, 7, 1);
    }
  for (int i = 0; i <= 4; i++)
    {
      bc_setbigcharpos (big, i, 0, 1);
      bc_setbigcharpos (big, i, 1, 1);
    }
  for (int i = 2; i <= 5; i++)
    {
      bc_setbigcharpos (big, 3, i, 1);
      bc_setbigcharpos (big, 4, i, 1);
    }

  return 0;
}

static int
generate_big_five (int *big)
{
  SET_FULL_LINE_0_1_3_4_6_7

  for (int i = 0; i < 1; i++)
    {
      bc_setbigcharpos (big, 5, 6 + i, 1);
      bc_setbigcharpos (big, 2, 0 + i, 1);
    }
  SET_PARALLELS (big, 0, 7, 0)

  return 0;
}

static int
generate_big_six (int *big)
{
  SET_FULL_LINE_0_1_3_4_6_7
  SET_PARALLELS (big, 0, 7, 0)

  bc_setbigcharpos (big, 3, 7, 0);

  for (int i = 0; i <= 1; i++)
    {
      bc_setbigcharpos (big, 2, i, 1);
      bc_setbigcharpos (big, 5, i, 1);
      bc_setbigcharpos (big, 5, 6 + i, 1);
    }

  return 0;
}

static int
generate_big_seven (int *big)
{
  for (int i = 0; i <= 5; i++)
    {
      bc_setbigcharpos (big, 0, i, 1);
      bc_setbigcharpos (big, 1, i, 1);
    }
  for (int i = 2; i <= 7; i++)
    {
      bc_setbigcharpos (big, i, 4, 1);
      bc_setbigcharpos (big, i, 5, 1);
    }
  for (int i = 4; i <= 7; i++)
    {
      if (i == 5 || i == 6)
        continue;
      bc_setbigcharpos (big, 3, i, 1);
      bc_setbigcharpos (big, 4, i, 1);
    }
  bc_setbigcharpos (big, 2, 0, 1);
  bc_setbigcharpos (big, 2, 1, 1);

  return 0;
}

static int
generate_big_eight (int *big)
{
  SET_FULL_LINE_0_1_3_4_6_7
  SET_PARALLELS (big, 0, 7, 0)

  for (int i = 0; i <= 1; i++)
    {
      bc_setbigcharpos (big, 2, i, 1);
      bc_setbigcharpos (big, 2, 6 + i, 1);
      bc_setbigcharpos (big, 5, i, 1);
      bc_setbigcharpos (big, 5, 6 + i, 1);
    }

  return 0;
}

static int
generate_big_nine (int *big)
{
  SET_FULL_LINE_0_1_3_4_6_7
  SET_PARALLELS (big, 0, 7, 0)

  bc_setbigcharpos (big, 4, 0, 1);

  for (int i = 0; i <= 1; i++)
    {
      bc_setbigcharpos (big, 2, i, 1);
      bc_setbigcharpos (big, 2, 6 + i, 1);
      bc_setbigcharpos (big, 5, 6 + i, 1);
    }

  return 0;
}

static int
generate_big_A (int *big)
{
  SET_FULL_LINE_0_1
  SET_FULL_LINE_3_4

  bc_setbigcharpos (big, 0, 7, 0);
  bc_setbigcharpos (big, 0, 0, 0);

  for (int i = 0; i <= 1; i++)
    {
      bc_setbigcharpos (big, 2, i, 1);
      bc_setbigcharpos (big, 2, 6 + i, 1);
    }
  for (int i = 5; i <= 7; i++)
    {
      bc_setbigcharpos (big, i, 0, 1);
      bc_setbigcharpos (big, i, 1, 1);
      bc_setbigcharpos (big, i, 6, 1);
      bc_setbigcharpos (big, i, 7, 1);
    }

  return 0;
}

static int
generate_big_B (int *big)
{
  SET_FULL_LINE_0_1_3_4_6_7

  bc_setbigcharpos (big, 0, 7, 0);
  bc_setbigcharpos (big, 7, 7, 0);
  bc_setbigcharpos (big, 3, 7, 0);
  bc_setbigcharpos (big, 4, 7, 0);

  for (int i = 2; i <= 5; i += 3)
    for (int j = 0; j <= 1; j++)
      {
        bc_setbigcharpos (big, i, j, 1);
        bc_setbigcharpos (big, i, 6 + j, 1);
      }

  return 0;
}

static int
generate_big_C (int *big)
{
  SET_FULL_LINE_0_1
  SET_FULL_LINE_6_7
  SET_PARALLELS (big, 0, 7, 0)

  for (int i = 2; i <= 5; i++)
    for (int j = 0; j <= 1; j++)
      bc_setbigcharpos (big, i, j, 1);

  return 0;
}

static int
generate_big_D (int *big)
{
  SET_FULL_LINE_0_1
  SET_FULL_LINE_6_7

  bc_setbigcharpos (big, 0, 7, 0);
  bc_setbigcharpos (big, 7, 7, 0);

  for (int i = 2; i <= 5; i++)
    for (int j = 6; j <= 7; j++)
      bc_setbigcharpos (big, i, j, 1);

  return 0;
}

static int
generate_big_E (int *big)
{
  SET_FULL_LINE_0_1_3_4_6_7

  for (int i = 0; i <= 1; i++)
    {
      bc_setbigcharpos (big, 2, i, 1);
      bc_setbigcharpos (big, 5, i, 1);
    }

  return 0;
}

static int
generate_big_F (int *big)
{
  SET_FULL_LINE_0_1
  SET_FULL_LINE_3_4

  bc_setbigcharpos (big, 2, 0, 1);
  bc_setbigcharpos (big, 2, 1, 1);

  for (int i = 4; i <= 7; i++)
    {
      for (int j = 0; j <= 1; j++)
        bc_setbigcharpos (big, i, j, 1);
    }

  return 0;
}

static int
generate_big_plus (int *big)
{
  SET_FULL_LINE_3_4

  for (int i = 0; i <= 7; i++)
    {
      bc_setbigcharpos (big, i, 3, 1);
      bc_setbigcharpos (big, i, 4, 1);
    }

  return 0;
}

static int
generate_big_minus (int *big)
{
  SET_FULL_LINE_3_4

  return 0;
}

int
generate_big_char (int *big, int count)
{
  int (*function_generate_big_char[]) (int *)
      = { generate_big_zero,  generate_big_one,   generate_big_two,
          generate_big_three, generate_big_four,  generate_big_five,
          generate_big_six,   generate_big_seven, generate_big_eight,
          generate_big_nine,  generate_big_A,     generate_big_B,
          generate_big_C,     generate_big_D,     generate_big_E,
          generate_big_F,     generate_big_plus,  generate_big_minus };

  for (int i = 0; i < count; i++)
    {
      function_generate_big_char[i](&big[i * 2]);
    }
  return 0;
}

int
main ()
{
  int big[34] = { 0 };
  int fd = open ("font.bin", O_RDWR);

  generate_big_char (big, 17);
  bc_bigcharwrite (fd, big, 34);

  return 0;
}
