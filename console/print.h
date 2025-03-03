#ifndef PRINT_H
#define PRINT_H

#include "myTerm.h"

void printCell (int address, enum colors fg, enum colors bg);
void printFlags ();
void printDecodedCommand (int value);
void printAccumulator ();
void printCounters ();

#endif