#include <stdio.h>

#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>

#include "myBigChars.h"
#include "myReadKey.h"
#include "mySimpleComputer.h"

#include "print.h"

struct itimerval nval1, oval1;

static void
init_generator ()
{
  /* generate 2 pulse per second */
  nval1.it_interval.tv_sec = 0;
  nval1.it_interval.tv_usec = 100000;
  nval1.it_value.tv_sec = 0;
  nval1.it_value.tv_usec = 100000;
}

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
  value &= ~(1 << 14);

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

  char bufCommand[3], bufOperand[3], dec[6];
  int value, sign, command, operand;
  int x = 64, y = 2;

  sc_accumulatorGet (&value);
  sc_commandDecode (value, &sign, &command, &operand);

  snprintf (bufCommand, 3, "%02X", command);
  snprintf (bufOperand, 3, "%02X", operand);
  snprintf (dec, 6, "%05d", (value) & ~(1 << 14));

  mt_gotoXY (x, y);

  write (fd, "sc: ", 4);
  write (fd, sign == 0 ? "+" : "-", 1);
  write (fd, bufCommand, 2);
  write (fd, bufOperand, 2);
  write (fd, " dec:", 5);
  write (fd, dec, 6);

  close (fd);
}

void
printCounters ()
{
  int fd = open ("/dev/stdout", O_WRONLY);
  if (fd == -1)
    return;

  int value, sign, command, operand;
  char bufCommand[3], bufOperand[3];

  sc_icounterGet (&value);
  sc_commandDecode (value, &sign, &command, &operand);
  snprintf (bufCommand, 3, "%02X", command);
  snprintf (bufOperand, 3, "%02X", operand);

  mt_gotoXY (73, 5);

  write (fd, "IC: ", 4);
  write (fd, sign == 0 ? "+" : "-", 1);
  write (fd, bufCommand, 2);
  write (fd, bufOperand, 2);

  close (fd);
}

void
printTactCounter (int tact)
{
  int fd = open ("/dev/stdout", O_WRONLY);
  if (fd == -1)
    return;

  mt_gotoXY (63, 5);

  char bufCounter[3];

  snprintf (bufCounter, 3, "%02X", tact);
  write (fd, "T: ", 3);
  write (fd, bufCounter, 2);

  close (fd);
}

void
printTerm (int address, int input)
{
  int fd = open ("/dev/stdout", O_WRONLY);
  if (fd == -1)
    return;

  int value, sign, command, operand;
  int y[] = { 20, 24 }, x = 69;
  static char buf[5][10];

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
      sc_memoryGet (address, &value);
      sc_commandDecode (value, &sign, &command, &operand);

      snprintf (buf[4], 10,
                address < 100 ? "%02d%c %c%02X%02X" : "%02d%c%c%02X%02X",
                address, '>', sign == 0 ? '+' : '-', command, operand);
    }
  else
    snprintf (buf[4], 5, "%02d<", address);

  for (int i = y[0], j = 0; i < y[1] + 1; i++, j++)
    {
      mt_gotoXY (x, i);
      write (fd, buf[j], 10);
    }
  if (input)
    {
      mt_gotoXY (73, 24);
      rk_readvalue (&value);
      init_generator();
      setitimer (ITIMER_REAL, &nval1, &oval1);
      memoryController(address, &value, SET);
      alarm (0);
      sc_memoryGet (address, &value);
      printCell (address, DEFAULT, DEFAULT);
      sc_commandDecode (value, &sign, &command, &operand);
      snprintf (&buf[4][5], 6, "%c%02X%02X", sign == 0 ? '+' : '-', command,
                operand);
    }
  close (fd);
}

void
printCommand ()
{
  int fd = open ("/dev/stdout", O_WRONLY);
  if (fd == -1)
    return;

  int reg;

  sc_regGet (4, &reg);
  if (reg)
    {
      mt_gotoXY (92, 5);
      write (fd, "! + FF : FF", 11);
      close (fd);
      return;
    }

  int address, value, sign, command, operand;
  char bufCommand[6], bufOperand[3];

  sc_icounterGet (&address);
  sc_memoryGet (address, &value);
  sc_commandDecode (value, &sign, &command, &operand);

  snprintf (bufCommand, 6, "%02X : ", command);
  snprintf (bufOperand, 3, "%02X", operand);

  mt_gotoXY (92, 5);

  write (fd, sign == 0 ? "  + " : "  - ", 4);
  write (fd, bufCommand, 5);
  write (fd, bufOperand, 2);

  close (fd);
}

void
printBigCell (int address, int big[36])
{
  int value;
  if (sc_memoryGet (address, &value) == -1)
    return;

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

  int fd = open ("/dev/stdout", O_WRONLY);
  if (fd == -1)
    return;
  char bufAddress[4];

  mt_gotoXY (63, 17);
  mt_setbgcolor (BG_BLUE);

  snprintf (bufAddress, 4, "%03d", address);

  write (fd, "Номер редактируемой ячейки: ", 52);
  write (fd, bufAddress, 3);

  mt_setdefaultcolor ();
  close (fd);
}

void
printHelpInformation ()
{
  int fd = open ("/dev/stdout", O_WRONLY);
  if (fd == -1)
    return;

  mt_gotoXY (80, 20);
  write (fd, "l - load", 8);
  mt_gotoXY (90, 20);
  write (fd, "s - save", 8);
  mt_gotoXY (100, 20);
  write (fd, "i - reset", 9);
  mt_gotoXY (80, 21);
  write (fd, "r - run", 7);
  mt_gotoXY (90, 21);
  write (fd, "t - step", 8);
  mt_gotoXY (80, 22);
  write (fd, "ESC - выход", 16);
  mt_gotoXY (80, 23);
  write (fd, "F5 - accumulator", 16);
  mt_gotoXY (80, 24);
  write (fd, "F6 - instruction counter", 24);

  close (fd);
}

int
printClearCell (int x, int y)
{
  int fd = open ("/dev/stdout", O_WRONLY);
  if (fd == -1)
    return -1;

  mt_gotoXY (x, y);
  write (fd, "     ", 5);
  close (fd);
  mt_gotoXY (x, y);

  return 0;
}

void
printCacheCell (int line, int output)
{
  if (line < 0 || line >= COUNT_LINE)
    return;

  int fd = open ("/dev/stdout", O_WRONLY);
  if (fd == -1)
    return;

  int value, num_line;
  char buf[4];

  mt_gotoXY (2, 20 + line);
  if (output == 1)
    {
      sc_cacheLineGet (10 * line, &value, &num_line);
      snprintf (buf, 4, "%02d", num_line);
      write (fd, buf, 4);
    }
  else
    write (fd, "xx", 2);
  write (fd, ":", 1);

  char bufOperand[3], bufCommand[3];
  int sign, command, operand;

  for (int i = 0; i < CACHE_LINE; i++)
    {
      sc_cacheLineGet (CACHE_LINE * line + i, &value, &num_line);
      sc_commandDecode (value, &sign, &command, &operand);
      snprintf (bufCommand, 3, "%02X", command);
      snprintf (bufOperand, 3, "%02X", operand);
      mt_gotoXY (7 + (6 * i), 20 + line);
      write (fd, sign == 0 ? "+" : "-", 1);
      write (fd, bufCommand, 2);
      write (fd, bufOperand, 2);
    }
  close (fd);
}