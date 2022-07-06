/*
File: symboltable.h
Author: Mikael Kindborg (mikael@kindborg.com)

A symbol table is an array of string pointers 
that maps symbol ids (indexes) to strings.
*/

struct __VSymbolTable
{
  char**  start;
  int     size;
  int     length;
}
VSymbolTable;

void SymbolTableInit(VSymbolTable* table, int size)
{
  table->start = (char**) PtrOffset(table, sizeof(VSymbolTable));
  table->size = size;
  table->length = 0;
}

// Return symbol string at index
char* SymbolTableGet(VSymbolTable* table, int index)
{
  if (index >= table->length)
  {
    GURU_MEDITATION(SYMBOL_TABLE_INDEX_OUT_OF_BOUNDS);
  }

  return table->start[index];
}

// Return index of symbol, return -1 if not found
int SymbolTableFind(VSymbolTable* table, char* symbol)
{
  for (int index = 0; index < table->length; ++ index)
  {
    char* str = SymbolTableGet(table, index);
    if (StrEquals(symbol, str))
    {
      return index; // Found symbol
    }
  }

  // Symbol not found
  return -1;
}

// Add symbol at the end of the array
// Return the index for the new symbol
// The symbol string is not copied
int SymbolTableAdd(VSymbolTable* table, char* symbol)
{
  if (table->length >= table->size)
  {
    GURU_MEDITATION(SYMBOL_TABLE_OUT_OF_SPACE);
  }

   table->start[table->length] = symbol;
   ++ table->length;
   return table->length;
}
