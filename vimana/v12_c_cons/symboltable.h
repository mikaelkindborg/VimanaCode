/*
File: symbols.h
Author: Mikael Kindborg (mikael@kindborg.com)

Lookup table for symbols. Used for parsing and printing.
*/

// TODO: Rename file to symboltable.h

VItem* SymbolTableNew()
{
  return ArrayNew(20);
}

void SymbolTableFree(VItem* array)
{
  for (int i = 0; i < ArrayLength(array); ++ i)
  {
    VItem* item = ArrayGet(array, i);
    StrFree(item->string);
  }

  ArrayFree(array);
}

// Finds or adds a symbol and returns symbol list and symbol index
int SymbolTableFindAdd(VItem* array, char* string)
{
  int index;

  for (index = 0; index < ArrayLength(array); ++ index)
  {
    VItem* item = ArrayGet(array, index);
    if (StrEquals(item->string, string))
    {
      return index; // Found existing symbol
    }
  }

  // Symbol not found, add it
  VItem newItem;
  newItem.string = StrCopy(string);
  ArraySet(array, index, &newItem);
  
  return index;
}

// Returns symbol string 
char* SymbolTableGetString(VItem* array, int index)
{
  VItem* item = ArrayGet(array, index);
  return item->string;
}

// Global symbol table

VItem* GSymbolTable = NULL;
int    GSymbolTableCounter = 0;

void GSymbolTableInit()
{
  if (NULL == GSymbolTable) GSymbolTable = SymbolTableNew();

  ++ GSymbolTableCounter;
}

void GSymbolTableRelease()
{
  if (GSymbolTableCounter > 0) -- GSymbolTableCounter;

  if (0 == GSymbolTableCounter && NULL != GSymbolTable)
  {
    SymbolTableFree(GSymbolTable);
    GSymbolTable = NULL;
  }
}

int GSymbolTableFindAdd(char* string)
{
  GSymbolTable = ArrayGrow(GSymbolTable, ArrayLength(GSymbolTable) + 10);
  return SymbolTableFindAdd(GSymbolTable, string);
}

char* GSymbolTableGetString(int index)
{
  return SymbolTableGetString(GSymbolTable, index);
}
