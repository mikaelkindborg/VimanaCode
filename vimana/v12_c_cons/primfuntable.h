/*
File: symbols.h
Author: Mikael Kindborg (mikael@kindborg.com)

Lookup table for primfuns.
*/

typedef struct __PrimFunEntry
{
  char*       name;
  VPrimFunPtr fun;
}
PrimFunEntry;

// Global primfun table
VArray* GPrimFunTable = NULL;

// Access a PrimFunEntry in the array
#define ArrayPrimFunEntryAt(array, index) \
  ((PrimFunEntry*)ArrayAt(array, index))

void PrimFunTableCreate()
{
  if (NULL == GPrimFunTable) 
  {
    GPrimFunTable = ArrayNew(sizeof(PrimFunEntry), 20);
  }
}

void PrimFunTableFree()
{
  for (int i = 0; i < ArrayLength(GPrimFunTable); ++ i)
  {
    PrimFunEntry* entry = ArrayPrimFunEntryAt(GPrimFunTable, i);
    //SysFree(entry->name);
  }

  ArrayFree(GPrimFunTable);
}

// name is not copied
void PrimFunAdd(char* name, VPrimFunPtr fun)
{
  // Grows array if needed
  GPrimFunTable = ArrayGrow(GPrimFunTable, ArrayLength(GPrimFunTable));

  PrimFunEntry* entry = ArrayPrimFunEntryAt(GPrimFunTable, ArrayLength(GPrimFunTable));

  entry->name = name; //StrCopy(name);
  entry->fun = fun;
}

int LookupPrimFun(char* name)
{
  for (int i = 0; i < ArrayLength(GPrimFunTable); ++ i)
  {
    PrimFunEntry* entry = ArrayPrimFunEntryAt(GPrimFunTable, i);
    if (StrEquals(entry->name, name))
    {
      return i;
    }
  }

  return -1;
}

VPrimFunPtr LookupPrimFunPtr(int index)
{
  return ArrayPrimFunEntryAt(GPrimFunTable, index)->fun;
}
