#include <stdlib.h>

#include "mySimpleComputer.h"

#define SC_ENCODE(x, y, z) x << 14 | y << 7 | z

#define SC_DECODE_SIGN(x) x >> 14
#define SC_DECODE_COMMAND(x) (x >> 7) & __INT8_MAX__
#define SC_DECODE_OPERAND(x) x &__INT8_MAX__

int
sc_commandEncode (int sign, int command, int operand, int *value)
{
  if (1 < sign || sign < 0 || __INT8_MAX__ < command || command < 0)
    return -1;
  if (__INT8_MAX__ < operand || operand < 0 || value == NULL)
    return -1;
  *value = 0;
  *value |= SC_ENCODE (sign, command, operand);
  return 0;
}

int
sc_commandDecode (int value, int *sign, int *command, int *operand)
{
  if (__SHRT_MAX__ < value || value < 0)
    return -1;
  if (sign == NULL || command == NULL || operand == NULL)
    return -1;

  *sign = SC_DECODE_SIGN (value);
  *command = SC_DECODE_COMMAND (value);
  *operand = SC_DECODE_OPERAND (value);

  return 0;
}

int
sc_commandValidate (int command)
{
  int pieceCommand[]
      = { 0, 1, 10, 11, 20, 21, 30, 31, 32, 33, 40, 41, 42, 43 };
  int sizePieceCommand = sizeof (pieceCommand) / sizeof (*pieceCommand);

  for (int i = 0; i < sizePieceCommand; i++)
    if (command == pieceCommand[i])
      return 0;
  for (int i = 51; i <= 76; i++)
    if (command == i)
      return 0;
  return 1;
}
