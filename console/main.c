#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

#include "myBigChars.h"
#include "myReadKey.h"
#include "mySimpleComputer.h"
#include "print.h"

#define SIZE_MEMORY 128

int big[36];

int
file_save_load (enum keys key)
{
  int x;

  mt_gotoXY (0, 26);

  switch (key)
    {
    case key_l:
      write (STDOUT_FILENO, "Введите имя файла для загрузки: ", 58);
      x = bc_strlen ("Введите имя файла для загрузки: ");
      break;
    case key_s:
      write (STDOUT_FILENO, "Введите имя файла для сохранения: ", 62);
      x = bc_strlen ("Введите имя файла для сохранения: ");
      break;
    default:
      return -1;
    }

  char *filename = calloc (sizeof (char), 256);
  if (filename == NULL)
    return -1;

  mt_setcursorvisible (0);
  rk_mytermsave ();

  for (int i = 0, count_byte = 0;;)
    {
      char buf[16] = "\0";

      rk_mytermregime (0, 0, 0, 1, 1);
      count_byte = read (STDIN_FILENO, buf, 15);
      if (strcmp (buf, "\n") == 0 || i >= 254 || count_byte < 0)
        break;
      if (strcmp (buf, "\177") == 0)
        {
          mt_gotoXY (x + 1, 26);
          filename[0] = '\0';
          i = 0;
          write (STDOUT_FILENO, "\e[K", 3);
          continue;
        }

      strcat (filename, buf);
      i += count_byte;
    }
  mt_setcursorvisible (1);
  rk_mytermrestore ();
  switch (key)
    {
    case key_l:
      sc_memoryLoad (filename);
      break;
    case key_s:
      sc_memorySave (filename);
    default:
    }
  free (filename);
  mt_gotoXY (0, 26);
  mt_delline ();

  return 0;
}

int
move (enum keys key, unsigned int *addr)
{
  unsigned int address = *addr;
  int value;

  printCell (address, DEFAULT, DEFAULT);
  switch (key)
    {
    case key_up:
      address -= 10;
      if (address > 127)
        {
          address = (address % 128) + 2;
          if (address == 128 || address == 129)
            address -= 10;
        }
      break;

    case key_down:
      address += 10;
      if (address >= 128 && address < 130)
        address = 8 + (address & 1);
      else if (address >= 130)
        address = (address & 0XF) - 2;
      break;

    case key_left:
      address--;
      address %= 128;
      break;

    case key_right:
      address++;
      address %= 128;
      break;

    default:
      break;
    }
  printCell (address, FG_WHITE, BG_BLACK);
  printBigCell (address, big);
  sc_memoryGet (address, &value);
  printDecodedCommand (value);
  *addr = address;
  return 0;
}

int
main (int argv, char *argc[])
{
  int fd, count;

  if (argv >= 2)
    {
      fd = open (argc[1], O_RDONLY);
      if (fd == -1)
        {
          printf ("Не удается открыть файл, указанный в параметрах\n");
          return -1;
        }
    }
  else
    {
      fd = open ("font.bin", O_RDONLY);
      if (fd == -1)
        {
          printf ("Не удается найти/открыть файл font.bin\n");
          return -1;
        }
    }
  bc_bigcharread (fd, big, 36, &count);

  close (fd);

  fd = open ("/dev/stdout", O_RDWR);
  if (fd == -1)
    {
      printf ("Ошибка! Стандартный поток вывода закрыт\n");
      return 1;
    }
  close (fd);

  int rows, cols;

  mt_getscreensize (&rows, &cols);
  if (rows < 27 || cols < 108)
    {
      printf ("Ошибка! Увеличьте размер терминала\n");
      return 1;
    }

  sc_memoryInit ();
  sc_regInit ();
  sc_accumulatorInit ();
  sc_icounterInit ();

  mt_clrscr ();
  mt_setcursorvisible (1);

  for (int i = 0; i < 10; i++)
    sc_memorySet (i * 10, i % 2 == 0 ? i * 15 : i * 30);

  sc_memorySet (8, __SHRT_MAX__);

  for (int i = 0; i < 128; i++)
    printCell (i, DEFAULT, DEFAULT);

  printFlags ();
  printAccumulator ();
  printCounters ();
  printCommand ();
  printHelpInformation ();

  bc_box (1, 1, 60, 14, DEFAULT, FG_WHITE, " Оперативная память ", BG_RED,
          DEFAULT);
  bc_box (1, 16, 60, 2, DEFAULT, FG_WHITE, " Редактируемая ячейка (формат) ",
          BG_RED, FG_WHITE);
  bc_box (62, 1, 22, 2, DEFAULT, FG_WHITE, " Аккумулятор ", BG_RED, DEFAULT);
  bc_box (85, 1, 24, 2, DEFAULT, FG_WHITE, " Регистр флагов ", BG_RED,
          DEFAULT);
  bc_box (62, 4, 22, 2, DEFAULT, FG_WHITE, " Счетчик команд ", BG_RED,
          DEFAULT);
  bc_box (85, 4, 24, 2, DEFAULT, FG_WHITE, " Команда ", BG_RED, DEFAULT);
  bc_box (62, 7, 47, 11, DEFAULT, FG_WHITE,
          " Редактируемая ячейка (увеличено) ", BG_RED, FG_WHITE);
  bc_box (68, 19, 10, 6, DEFAULT, FG_WHITE, " IN--OUT ", BG_GREEN, FG_WHITE);
  bc_box (79, 19, 30, 6, DEFAULT, FG_WHITE, " Клавиши ", BG_GREEN, FG_WHITE);
  bc_box (1, 19, 66, 6, DEFAULT, FG_WHITE, " Кеш процессора ", BG_GREEN,
          FG_WHITE);

  unsigned int address = 0;
  enum keys key;
  struct termios t;
  int value;

  tcgetattr (STDIN_FILENO, &t);

  printCell (address, FG_WHITE, BG_BLACK);
  printBigCell (address, big);

  sc_memoryGet (address, &value);
  printDecodedCommand (value);

  if (rk_mytermregime (0, 0, 0, 1, 1))
    return -1;

  while (1)
    {
      if (rk_readkey (&key))
        return -1;

      if (key == key_esape)
        break;

      switch (key)
        {
        case key_up:
        case key_down:
        case key_left:
        case key_right:
          move (key, &address);
          break;

        case key_enter:
          printClearCell (2 + (address % 10 * 6), 2 + address / 10);
          sc_memoryGet (address, &value);
          rk_readvalue (&value, 100);
          sc_memorySet (address, value);
          sc_memoryGet (address, &value);
          printDecodedCommand (value);
          printCell (address, FG_WHITE, BG_BLACK);
          printBigCell (address, big);
          break;

        case key_f5:
          printClearCell (68, 2);
          sc_accumulatorGet (&value);
          rk_readvalue (&value, 100);
          sc_accumulatorSet (value);
          printAccumulator ();
          break;

        case key_f6:
          printClearCell (77, 5);
          sc_icounterGet (&value);
          rk_readvalue (&value, 100);
          sc_icounterSet (value);
          printCounters ();
          printAccumulator ();
          break;

        case key_l:
          file_save_load (key_l);
          for (int i = 0; i < SIZE_MEMORY; i++)
            printCell (i, DEFAULT, DEFAULT);
          sc_memoryGet (address, &value);
          printDecodedCommand (value);
          printCell (address, FG_WHITE, BG_BLACK);
          printBigCell (address, big);
          break;

        case key_s:
          file_save_load (key_s);
          break;

        case key_i:
          sc_memoryInit ();
          sc_regInit ();
          for (int i = 0; i < SIZE_MEMORY; i++)
            printCell (i, DEFAULT, DEFAULT);
          printFlags ();
          sc_memoryGet (address, &value);
          printDecodedCommand (value);
          printCell (address, FG_WHITE, BG_BLACK);
          printBigCell (address, big);
          break;

        default:
        };
    }
  tcsetattr (STDIN_FILENO, TCSAFLUSH, &t);
  mt_setcursorvisible (0);
  mt_gotoXY (0, 27);

  return 0;
}