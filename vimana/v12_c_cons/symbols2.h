/*
File: symbols.h
Author: Mikael Kindborg (mikael@kindborg.com)

Lookup table for symbols. Used for parsing and printing.
*/

// Finds or adds a symbol and returns symbol list and symbol index
int SymbolFindAdd(VItem* array, char* string, VMem* mem)
{
  VItem* item;
  int    index;

  //printf("array length: %i\n", ArrayLength(array));

  for (index = 0; index < ArrayLength(array); ++ index)
  {
    item = ArrayGet(array, index);
    char* str = (char*) MemItemString(mem, item);
    if (StrEquals(string, str))
    {
      return index; // Found existing symbol
    }
  }

  // Symbol not found, add it
  item = MemAllocItem(mem);
  MemItemSetString(mem, item, string);
  ArraySet(array, index, item);
  
  //printf("Added to symbol table: %s\n", (char*)MemItemString(mem, item));

  return index;
}

// Returns symbol string 
char* SymbolGetString(VItem* array, int index, VMem* mem)
{
  VItem* item = ArrayGet(array, index);
  return MemItemString(mem, item);
}
