#ifndef SC_ACCUMULATOR_H
#define SC_ACCUMULATOR_H
int sc_accumulatorInit ();
int sc_accumulatorSet (int value);
int sc_accumulatorGet (int *value);
#endif

#ifndef SC_COMMAND_H
#define SC_COMMAND_H
enum
{
  NOP = 0,
  CPUINFO = 1,
  READ = 10,
  WRITE = 11,
  LOAD = 20,
  STORE = 21,
  ADD = 30,
  SUB = 31,
  DIVIDE = 32,
  MUL = 33,
  JUMP = 40,
  JNEG = 41,
  JZ = 42,
  HALT = 43,
  SUBC = 66,
  LOGLC = 67,
};

int sc_commandEncode (int sign, int command, int operand, int *value);
int sc_commandDecode (int value, int *sign, int *command, int *operand);
int sc_commandValidate (int command);
#endif

#ifndef SC_ICOUNTER_H
#define SC_ICOUNTER_H
int sc_icounterInit ();
int sc_icounterSet (int value);
int sc_icounterGet (int *value);
#endif

#ifndef SC_MEMORY_H
#define SC_MEMORY_H
int sc_memoryInit ();
int sc_memorySet (int address, int value);
int sc_memoryGet (int address, int *value);
int sc_memorySave (char *filename);
int sc_memoryLoad (char *filename);
#endif

#ifndef SC_REG_H
#define SC_REG_H
int sc_regInit ();
int sc_regSet (int registr, int value);
int sc_regGet (int registr, int *value);
#endif