#include <stdio.h>

#include <fcntl.h>
#include <unistd.h>

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

  mt_gotoXY (96, 2);

  for (int i = 0; i < 5; i++)
    {
      sc_regGet (i, &value);
      write (fd, value == 0 ? "_" : &valueError[i], 1);
      write (fd, " ", 1);
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
  int x = 70, y = 2;

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

  int value = 0;
  sc_icounterGet (&value);
  printf ("Значение счётчика команд: %d\n", value);
}