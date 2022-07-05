/*
File: SymbolTable.h
Author: Mikael Kindborg (mikael@kindborg.com)

The symbol table is an array of string pointers.
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
  VArray* array = SysAlloc(sizeof(VSymbolTable) + (size * sizeof(char*));

  table->start = (char**) PtrOffset(array, sizeof(VSymbolTable));
  table->size = size;
  table->length = 0;
}

char* SymbolTableGet(VSymbolTable* table, int index)
{
  if (index >= table->size)
  {
    GURU_MEDITATION(SYMBOL_TABLE_OUT_OF_BOUNDS);
  }

  return table[index];
}

// Return index of added item
int SymbolTableAdd(VSymbolTable* table, char* str)
{
   table[array->length] = str;
   ++ table->length;
   return table->length;
}

int SymbolTableAddIfNew(VSymbolTable* table, char* str)
{
   table[array->length] = str;
   ++ table->length;
   return table->length;
}

// Return index of existing or added string
// The string is not copied
int SymbolTableFind(VSymbolTable* table, char* str)
{
  for (int index = 0; index < table->length; ++ index)
  {
    char* str2 = SymbolTableGet(table, index);
    if (StrEquals(str, str2))
    {
      return index; // Found existing symbol
    }
  }

  // Symbol not found, add it
  return SymbolTableAdd(table, str);
}
