/*
File: primfuntable.h
Author: Mikael Kindborg (mikael@kindborg.com)

Lookup table for primfuns.
*/

typedef void (*VPrimFunPtr) (VInterp*);

typedef struct __VPrimFunEntry
{
  char*       name;
  VPrimFunPtr fun;
}
VPrimFunEntry;

// Global primfun table
VPrimFunEntry* GlobalPrimFunTable = NULL;

// Number of entries in the table
int GlobalPrimFunTableSize = 0;

void PrimFunTableInit(void* table)
{
  GlobalPrimFunTable = table;
}

// Add a primfun to the table
// Param name is not copied
// Updates index of last entry
void PrimFunTableAdd(char* name, VPrimFunPtr fun)
{
  VPrimFunEntry* entry = GPrimFunTable[GlobalPrimFunTableSize];

  entry->name = name;
  entry->fun = fun;

  ++ GlobalPrimFunTableSize;
}

VPrimFunEntry* PrimFunTableGetEntry(int index)
{
  return GlobalPrimFunTable[index];
}

/*
VPrimFunPtr LookupPrimFunPtr(int index)
{
  return PrimFunTableGetEntry(index)->fun;
}
*/

// Lookup index of entry by name
// Return -1 if the entry does not exist
int PrimFunTableLookupByName(char* name)
{
  for (int i = 0; i < GlobalPrimFunTableSize; ++ i)
  {
    VPrimFunEntry* entry = PrimFunTableGetEntry(i);
    if (name == entry->name)
    {
      return i;
    }
  }

  return -1;
}

// Lookup index of entry by name
// Return -1 if the entry does not exist
int PrimFunTableLookupByFunPtr(VPrimFunPtr funPtr)
{
  for (int i = 0; i < GlobalPrimFunTableSize; ++ i)
  {
    VPrimFunEntry* entry = PrimFunTableGetEntry(i);
    if (funPtr == entry->fun)
    {
      return i;
    }
  }

  return -1;
}
