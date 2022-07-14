/*
File: symboltable.h
Author: Mikael Kindborg (mikael@kindborg.com)

The symbol table maps symbol ids (indexes) to strings.
*/

// Global symbol table
static char** GlobalSymbolTable;
static int    GlobalSymbolTableSize;
static int    GlobalSymbolTableMaxSize;

void SymbolTableInit(void* mem, int size)
{
  GlobalSymbolTable = mem;
  GlobalSymbolTableSize = 0;
  GlobalSymbolTableMaxSize = size;
}

int SymbolTableByteSize()
{
  return sizeof(char*) * GlobalSymbolTableMaxSize;
}

int SymbolTableMaxSize()
{
  return GlobalSymbolTableMaxSize;
}

// Add symbol at the end of the array
int SymbolTableAdd(char* symbol)
{
  if (GlobalSymbolTableSize < GlobalSymbolTableMaxSize)
  {
    GlobalSymbolTable[GlobalSymbolTableSize] = symbol;
    ++ GlobalSymbolTableSize;
  }
  else
  {
    GURU_MEDITATION(SYMBOL_TABLE_OUT_OF_MEMORY);
  }
  return GlobalSymbolTableSize;
}

char* SymbolTableGet(int index)
{
  return GlobalSymbolTable[index];
}

// Return index of symbol, return -1 if not found
int SymbolTableFind(char* symbol)
{
  for (int index = 0; index < GlobalSymbolTableSize; ++ index)
  {
    char* name = SymbolTableGet(index);
    if (StrEquals(symbol, name))
    {
      return index; // Found symbol
    }
  }

  // Symbol not found
  return -1;
}
