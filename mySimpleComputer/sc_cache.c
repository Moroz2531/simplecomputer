#include <stdio.h>

#include "mySimpleComputer.h"

int cache_memory[SIZE_CACHE_MEMORY];
int num_line[COUNT_LINE], hit_counter[COUNT_LINE];

int
sc_cacheInit ()
{
  for (int i = 0; i < SIZE_CACHE_MEMORY; i++)
    cache_memory[i] = 0;
  for (int i = 0; i < COUNT_LINE; i++)
    {
      num_line[i] = 0;
      hit_counter[i] = 0;
    }
  return 0;
}

int
sc_cacheLineGet (int address, int *value, int *number_line)
{
  if (address < 0 || address >= SIZE_CACHE_MEMORY || value == NULL
      || number_line == NULL)
    return -1;

  *value = cache_memory[address];
  *number_line = num_line[address / 10];
  return 0;
}

static int
sc_cacheCheckAddr (int address, int *result)
{
  if (address < 0 || address >= SIZE_MEMORY || result == NULL)
    return -1;

  int line = address - (address % 10);

  *result = 0;
  for (int i = 0; i < COUNT_LINE; i++)
    {
      if (num_line[i] == line)
        {
          *result = 1;
          break;
        }
    }
  return 0;
}

static int
restore_hit_counter ()
{
  int flag_correct_hit_counter = 0;

  for (int i = 0; i < COUNT_LINE; i++)
    if (hit_counter[i] == 1)
      {
        flag_correct_hit_counter = 1;
        break;
      }
  if (flag_correct_hit_counter == 0)
    {
      for (int i = 0; i < COUNT_LINE; i++)
        {
          if (hit_counter[i] == 0)
            continue;
          hit_counter[i]--;
        }
    }
  return 0;
}

static int
get_max_value_hit_counter ()
{
  int max_value = hit_counter[0];

  for (int i = 1; i < COUNT_LINE; i++)
    if (max_value < hit_counter[i])
      max_value = hit_counter[i];
  return max_value;
}

int
sc_cacheLineLoad (int address)
{
  if (address < 0 || address >= SIZE_MEMORY)
    return -1;

  int result;
  sc_cacheCheckAddr (address, &result);
  if (result)
    return 0;

  int number_line = address - (address % 10);
  int min_line = 0, max_value = hit_counter[0];

  for (int hit_min = hit_counter[0], i = 1; i < COUNT_LINE; i++)
    {
      if (hit_min > hit_counter[i])
        {
          min_line = i;
          hit_min = hit_counter[i];
        }
      if (max_value < hit_counter[i])
        max_value = hit_counter[i];
    }
  hit_counter[min_line] = max_value + 1;
  num_line[min_line] = number_line;

  restore_hit_counter ();

  for (int i = number_line; i < (number_line + 10) && i < SIZE_MEMORY; i++)
    sc_memoryGet (i, &cache_memory[min_line + (i % 10)]);
  return 0;
}

int
sc_cacheSet (int address, int value)
{
  int result;
  if (sc_cacheCheckAddr (address, &result))
    return -1;
  if (result == 0)
    return -2;

  int number_line = address - (address % 10);
  const int sz = CACHE_LINE;

  for (int i = 0; i < COUNT_LINE; i++)
    {
      if (number_line == num_line[i])
        {
          number_line = i;
          break;
        }
    }
  if (sc_memorySet (address, value))
    return -1;
  cache_memory[number_line * sz + (address % sz)] = value;

  int max_value = get_max_value_hit_counter ();

  if (max_value != hit_counter[number_line])
    {
      hit_counter[number_line] = max_value + 1;
      restore_hit_counter ();
    }

  return 0;
}

int
sc_cacheGet (int address, int *value)
{
  int result;
  if (value == NULL || sc_cacheCheckAddr (address, &result))
    return -1;
  if (result == 0)
    return -2;

  int number_line = address - (address % 10);
  const int sz = CACHE_LINE;

  for (int i = 0; i < COUNT_LINE; i++)
    {
      if (number_line == num_line[i])
        {
          number_line = i;
          break;
        }
    }
  *value = cache_memory[number_line * sz + (address % sz)];

  int max_value = get_max_value_hit_counter ();

  if (max_value != hit_counter[number_line])
    {
      hit_counter[number_line] = max_value + 1;
      restore_hit_counter ();
    }

  return 0;
}

int
sc_getLine (int address, int *line)
{
  int result;
  if (sc_cacheCheckAddr (address, &result))
    return -1;
  int l = address - (address % 10);
  for (int i = 0; i < COUNT_LINE; i++)
    if (l == hit_counter[i])
      {
        *line = i;
        break;
      }
  return 0;
}
