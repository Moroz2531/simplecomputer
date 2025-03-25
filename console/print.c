#include <stdio.h>

#include <fcntl.h>
#include <unistd.h>

#include "myBigChars.h"
#include "mySimpleComputer.h"

#include "print.h"

void
printCell (int address, enum colors fg, enum colors bg)
{
  int value = 0;
  if (sc_memoryGet (address, &value))
    return;

  int fd = open ("/dev/stdout", O_WRONLY);
  if (fd == -1)
    return;

  int x = 2 + (address % 10 * 6), y = 2 + address / 10;

  mt_gotoXY (x, y);
  mt_setbgcolor (bg);
  mt_setfgcolor (fg);

  char bufOperand[3], bufCommand[3];
  int sign, command, operand;

  if (sc_commandDecode (value, &sign, &command, &operand))
    return;

  snprintf (bufCommand, 3, "%02X", command);
  snprintf (bufOperand, 3, "%02X", operand);

  write (fd, sign == 0 ? "+" : "-", 1);
  write (fd, bufCommand, 2);
  write (fd, bufOperand, 2);

  mt_setdefaultcolor ();

  close (fd);
}

void
printFlags ()
{
  int fd = open ("/dev/stdout", O_WRONLY);
  if (fd == -1)
    return;

  char valueError[] = "P0MTE";
  int value;

  mt_gotoXY (91, 2);

  for (int i = 0; i < 5; i++)
    {
      sc_regGet (i, &value);
      write (fd, value == 0 ? "_" : &valueError[i], 1);
      if (i != 4)
        write (fd, "  ", 2);
    }
  close (fd);
}

void
printDecodedCommand (int value)
{
  if (value > __SHRT_MAX__ || value < 0)
    return;

  int fd = open ("/dev/stdout", O_WRONLY);
  if (fd == -1)
    return;

  char bin[16], dec[8], oct[8], hex[5];

  for (int i = 0; i < 15; i++)
    bin[i] = (1 & (value >> (14 - i))) == 1 ? '1' : '0';

  snprintf (dec, 8, "%05d |", value);
  snprintf (oct, 8, "%05o |", value);
  snprintf (hex, 5, "%04X", value);

  int addressX[] = { 2, 15, 28, 41 }, y = 17;

  mt_gotoXY (addressX[0], y);
  write (fd, "dec: ", 5);
  write (fd, dec, 8);

  mt_gotoXY (addressX[1], y);
  write (fd, "oct: ", 5);
  write (fd, oct, 8);

  mt_gotoXY (addressX[2], y);
  write (fd, "hex: ", 5);
  write (fd, hex, 5);

  mt_gotoXY (addressX[3], y);
  write (fd, "bin: ", 5);
  write (fd, bin, 15);

  close (fd);
}

void
printAccumulator ()
{
  int fd = open ("/dev/stdout", O_WRONLY);
  if (fd == -1)
    return;

  char bufCommand[3], bufOperand[3], hex[5];
  int value, sign, command, operand;
  int x = 64, y = 2;

  sc_accumulatorGet (&value);
  sc_commandDecode (value, &sign, &command, &operand);

  snprintf (bufCommand, 3, "%02X", command);
  snprintf (bufOperand, 3, "%02X", operand);
  snprintf (hex, 5, "%04X", value);

  mt_gotoXY (x, y);

  write (fd, "sc: ", 4);
  write (fd, sign == 0 ? "+" : "-", 1);
  write (fd, bufCommand, 2);
  write (fd, bufOperand, 2);
  write (fd, " hex: ", 6);
  write (fd, hex, 5);

  close (fd);
}

void
printCounters ()
{
  int fd = open ("/dev/stdout", O_WRONLY);
  if (fd == -1)
    return;

  int value;
  char buf[5];

  sc_icounterGet (&value);

  snprintf (buf, 5, "%04X", value);

  int icounterX = 73, icounterY = 5;

  mt_gotoXY (icounterX, icounterY);

  write (fd, "IC: ", 4);
  write (fd, "+", 1);
  write (fd, buf, 4);

  close (fd);
}

void
printTerm (int address, int input)
{
  int fd = open ("/dev/stdout", O_WRONLY);
  if (fd == -1)
    return;

  int value, sign, command, operand;

  sc_memoryGet (address, &value);
  sc_commandDecode (value, &sign, &command, &operand);

  static char buf[5][10];
  int y[] = { 20, 24 }, x = 69;

  for (int i = 0; i < 4; i++)
    {
      for (int j = 0; j < 10; j++)
        {
          buf[i][j] = buf[i + 1][j];
          buf[i + 1][j] = ' ';
        }
    }

  if (input == 0)
    {
      snprintf (buf[4], 10,
                address < 100 ? "%02d%c %c%02X%02X" : "%02d%c%c%02X%02X",
                address, '>', sign == 0 ? '+' : '-', command, operand);
    }
  else
    snprintf (buf[4], 5, "%2d<", address);

  for (int i = y[0], j = 0; i < y[1] + 1; i++, j++)
    {
      mt_gotoXY (x, i);
      write (fd, buf[j], 10);
    }
  close (fd);
}

void
printCommand ()
{
  int fd = open ("/dev/stdout", O_WRONLY);
  if (fd == -1)
    return;

  int address, value, sign, command, operand;
  char bufCommand[6], bufOperand[3];

  sc_icounterGet (&address);

  if (address < 0 || address > 127)
    {
      mt_gotoXY (92, 5);
      write (fd, "! + FF : FF", 11);
      close (fd);
      return;
    }

  sc_memoryGet (address, &value);
  sc_commandDecode (value, &sign, &command, &operand);

  if (sc_commandValidate (command))
    {
      mt_gotoXY (92, 5);
      write (fd, "! + FF : FF", 11);
      close (fd);
      return;
    }

  snprintf (bufCommand, 6, "%02X : ", command);
  snprintf (bufOperand, 3, "%02X", operand);

  mt_gotoXY (94, 5);

  write (fd, sign == 0 ? "+ " : "- ", 2);
  write (fd, bufCommand, 5);
  write (fd, bufOperand, 2);

  close (fd);
}

void
printBigCell (int address, int big[36])
{
  int value;
  sc_memoryGet (address, &value);

  int sign, command, operand;
  sc_commandDecode (value, &sign, &command, &operand);

  switch (sign)
    {
    case 0:
      bc_printbigchar (&big[32], 64, 9, DEFAULT, DEFAULT);
      break;
    case 1:
      bc_printbigchar (&big[34], 64, 9, DEFAULT, DEFAULT);
      break;
    }
  bc_printbigchar (&big[2 * (command >> 4)], 73, 9, DEFAULT, DEFAULT);
  bc_printbigchar (&big[2 * (command & 15)], 82, 9, DEFAULT, DEFAULT);
  bc_printbigchar (&big[2 * (operand >> 4)], 91, 9, DEFAULT, DEFAULT);
  bc_printbigchar (&big[2 * (operand & 15)], 100, 9, DEFAULT, DEFAULT);
}