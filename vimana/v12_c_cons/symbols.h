/*
File: symbols.h
Author: Mikael Kindborg (mikael@kindborg.com)

Lookup table for symbols. Used for parsing and printing.
*/

// Adds or finds a symbol and returns symbol list and symbol index
int SymbolFindAdd(VItem* item, char* symbol, VMem* mem)
{
  // Check if this the initial empty item
  if (TypeNone == ItemType(item))
  {
    ItemSetString(item, symbol);
    printf("Add: %s\n", (char*)ItemData(item));
    return 0;
  }

  int index = 0;
  VItem* prev;

  while (item)
  {
    char* str = (char*) ItemData(item);
    if (StrEquals(symbol, str))
    {
      return index; // Found existing symbol
    }
    prev = item;
    item = MemItemNext(mem, item);
    ++ index;
  }

  // Add symbol
  item = MemAllocItem(mem);
  ItemSetString(item, symbol);
  MemCons(mem, prev, item);
  printf("Add: %s\n", (char*)ItemData(item));

  return index;
}

// Returns symbol string 
char* SymbolGetString(VItem* list, int index, VMem* mem)
{
  VItem* item = list;
  int n = 0;

  while (item)
  {
    printf("Get: %s\n", (char*)ItemData(item));
    if (n == index)
    {
      return (char*) ItemData(item); // Found
    }
    item = MemItemNext(mem, item);
    ++ n;
  }

  return NULL; // Not found
}
