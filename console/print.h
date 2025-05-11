#ifndef PRINT_H
#define PRINT_H

#include "myTerm.h"

void printCell (int address, enum colors fg, enum colors bg);
void printFlags ();
void printDecodedCommand (int value);
void printAccumulator ();
void printCounters ();
void printTactCounter (int tact);
void printTerm (int address, int input);
void printCommand ();
void printBigCell (int address, int big[36]);
void printHelpInformation ();
void printCacheCell (int line, int output);
int printClearCell (int x, int y);

#endif