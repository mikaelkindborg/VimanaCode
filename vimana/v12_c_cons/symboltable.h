/*
File: symbols.h
Author: Mikael Kindborg (mikael@kindborg.com)

Lookup table for symbols. Used for parsing and printing.
*/

// Global symbol table
VArray* GSymbolTable = NULL;

void SymbolTableCreate()
{
  if (NULL == GSymbolTable) 
  {
    GSymbolTable = ArrayNew(sizeof(char*), 20);
  }
}

void SymbolTableFree()
{
  for (int i = 0; i < ArrayLength(GSymbolTable); ++ i)
  {
    char* str = ArrayStringAt(GSymbolTable, i);
    SysFree(str);
  }

  ArrayFree(GSymbolTable);

  GSymbolTable = NULL;
}

// Finds or adds a string to the symbol table and 
// returns the symbol index
// string is copied
int SymbolTableFindAdd(char* string)
{
  // Grows array if needed
  GSymbolTable = ArrayGrow(GSymbolTable, ArrayLength(GSymbolTable));
  
  int index;

  for (index = 0; index < ArrayLength(GSymbolTable); ++ index)
  {
    char* str = ArrayStringAt(GSymbolTable, index);
    if (StrEquals(str, string))
    {
      return index; // Found existing symbol
    }
  }

  // Symbol not found, add it
  ArrayStringAt(GSymbolTable, index) = StrCopy(string);
  
  return index;
}

// Returns string for symbol at index
char* SymbolTableGet(int index)
{
  return ArrayStringAt(GSymbolTable, index);
}
