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
static VPrimFunEntry* GlobalPrimFunTable;
static int            GlobalPrimFunTableSize;

void PrimFunTableInit(void* mem)
{
  GlobalPrimFunTable = mem;
  GlobalPrimFunTableSize = 0;
}

int PrimFunTableByteSize()
{
  return sizeof(VPrimFunEntry) * GlobalPrimFunTableSize;
}

// Add a primfun to the table
// Param name is not copied
void PrimFunTableAdd(char* name, VPrimFunPtr fun)
{
  VPrimFunEntry* entry = & GlobalPrimFunTable[GlobalPrimFunTableSize];

  entry->name = name;
  entry->fun = fun;

  // Update index of last entry
  ++ GlobalPrimFunTableSize;
}

VPrimFunEntry* PrimFunTableGet(int index)
{
  return & GlobalPrimFunTable[index];
}

// Lookup index of entry by name
// Return -1 if the entry does not exist
int PrimFunTableLookupByName(char* name)
{
  for (int i = 0; i < GlobalPrimFunTableSize; ++ i)
  {
    VPrimFunEntry* entry = PrimFunTableGet(i);
    if (name == entry->name)
    {
      return i;
    }
  }

  return -1;
}

// Lookup index of entry by function pointer
// Return -1 if the entry does not exist
int PrimFunTableLookupByFunPtr(VPrimFunPtr funPtr)
{
  for (int i = 0; i < GlobalPrimFunTableSize; ++ i)
  {
    VPrimFunEntry* entry = PrimFunTableGet(i);
    if (funPtr == entry->fun)
    {
      return i;
    }
  }

  return -1;
}
