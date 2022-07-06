/*
File: symboltable.h
Author: Mikael Kindborg (mikael@kindborg.com)

The symbol table maps symbol ids (indexes) to strings.
*/

// Global symbol table
// This pointer is set elsewhere
static char** GlobalSymbolTable = NULL;

// Number of entries in the table
static int GlobalSymbolTableSize = 0;

// Max number of entries
static int GlobalSymbolTableMaxSize = 0;

void SymbolTableInit(void* table, int maxSize)
{
  GlobalSymbolTable = table;
  GlobalSymbolTableMaxSize = maxSize;
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
    GURU_MEDITATION(SYMBOL_TABLE_OUR_OF_MEMORY);
  }
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
