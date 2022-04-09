/*
File: symbols.h
Author: Mikael Kindborg (mikael@kindborg.com)

Lookup table for symbols. Used for parsing and printing.
*/

// Adds or finds a symbol and returns symbol list and symbol index
int SymbolFindAdd(VItem* list, char* symbol, VMem* mem)
{
  VItem* prev;

  int index = 0;
  VItem* item = MemItemFirst(mem, list);

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

  // Symbol not found, add it
  item = MemAllocItem(mem);
  ItemSetString(item, symbol);

  if (MemItemFirst(mem, list))
    MemItemSetNext(mem, prev, item);
  else
    MemItemSetFirst(mem, list, item);

  printf("Add: %s\n", (char*)ItemData(item));

  return index;
}

// Returns symbol string 
char* SymbolGetString(VItem* list, int index, VMem* mem)
{
  VItem* item = MemItemFirst(mem, list);
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
