#ifndef MEMORY_CONTROLLER_H
#define MEMORY_CONTROLLER_H

enum
{
  SET = 0,
  GET = 1,
};

int memoryController (int operand, int *value, int state);
#endif