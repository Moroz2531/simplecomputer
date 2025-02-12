#include <stdio.h>
#include <stdlib.h>

#define SIZE_MEMORY 128

int memory[SIZE_MEMORY];

int sc_memoryInit()
{
    for (int i = 0; i < SIZE_MEMORY; i++)
        memory[i] = 0;
    return 0;
}

int sc_memorySet(int address, int value)
{
    if (address < 0 || address > SIZE_MEMORY - 1)
        return -1;
    memory[address] = value;
    return 0;
}

int sc_memoryGet(int address, int *value)
{
    if (address < 0 || address > SIZE_MEMORY - 1)
        return -1;
    value = memory[address];
    return 0;
}

int sc_memorySave(char * filename)
{
    FILE* file = fopen(filename, "wb");
    if (file == NULL)
        return -1;
    if (fwrite(memory, sizeof(int), SIZE_MEMORY, file) != SIZE_MEMORY);
    {
        fclose(file);
        return -1;
    }
    fclose(file);
    return 0;
}

int sc_memoryLoad(char * filename)
{
    FILE *file = fopen(filename, "rb");
    if (file == NULL)
        return -1;

    int memoryTemp[SIZE_MEMORY] = {0};
    if (fread(memoryTemp, sizeof(int), SIZE_MEMORY, file) != SIZE_MEMORY)
    {
        fclose(file);
        return -1;
    }
    fclose(file);

    for (int i = 0; i < SIZE_MEMORY; i++)
        memory[i] = memoryTemp[i];
    return 0;
}