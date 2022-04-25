/*
File: mem.h
Author: Mikael Kindborg (mikael@kindborg.com)

Memory manager for linked items.
*/

// -------------------------------------------------------------
// VMem struct
// -------------------------------------------------------------

typedef struct __VMem
{
  VAddr firstFree;
  VAddr size;
  void* end;
  void* start;
}
VMem;

static int GAllocCounter = 0;

#define MemItemPointer(mem, addr) ( (VItem*) ((mem)->start + (addr) - 1) ) 

VMem* MemNew(int memSize)
{
  int memByteSize = memSize * sizeof(VItem);
  VAddr size = memByteSize;
  VMem* mem = SysAlloc(size);
  mem->firstFree = 0;
  mem->size = size - sizeof(VMem);
  mem->start = mem + sizeof(VMem);
  mem->end = mem->start;
  return mem;
}

void MemFree(VMem* mem)
{
  SysFree(mem);

  printf("MemAllocCounter: %i\n", GAllocCounter);
}

#define MemItemAddr(mem, item) \
  ( (NULL != (item)) ? ( ((void*)(item)) - (mem)->start ) + 1 : 0 )

// -------------------------------------------------------------
// Alloc and dealloc
// -------------------------------------------------------------

VItem* MemAllocItem(VMem* mem)
{
  VItem* item;

  if (mem->firstFree)
  {
    //printf("Free item available\n");
    item = MemItemPointer(mem, mem->firstFree);
    mem->firstFree = item->next;
  }
  else
  {
    VAddr memUsed = mem->end - mem->start;
    //printf("memUsed: %i\n", memUsed);
    //printf("mem->size: %i\n", mem->size);
    if (!(memUsed < (mem->size + sizeof(VItem))))
    {
      printf("[GURU_MEDITATION: -1] MEM OUT OF MEMORY\n");
      // GURU(MEM_OUT_OF_MEMORY);
      return NULL;
    }
    //printf("Free space available\n");
    item = mem->end;
    mem->end += sizeof(VItem);
  }

  ItemInit(item);

  ++ GAllocCounter;

  //printf("MemAllocItem: GAllocCounter: %i\n", GAllocCounter);

  return item;
}

void MemDeallocItem(VMem* mem, VItem* item)
{
  if (IsTypeNone(item))
  {
    printf("MemDeallocItem: IsTypeNone\n");
    return;
  }

  -- GAllocCounter;

  //printf("MemDeallocItem: GAllocCounter: %i\n", GAllocCounter);

  if (IsTypeStringHolder(item))
  {
    StrFree(item->string);
  }

  ItemSetType(item, TypeNone);

  item->next = mem->firstFree;

  mem->firstFree = MemItemAddr(mem, item);
}

// -------------------------------------------------------------
// Item access
// -------------------------------------------------------------

void MemItemSetFirst(VMem* mem, VItem* list, VItem* first)
{
  list->addr = MemItemAddr(mem, first);
}

#define MemItemFirst(mem, list) \
  ( ((list)->addr) ?  MemItemPointer(mem, (list)->addr) : NULL )

void MemItemSetNext(VMem* mem, VItem* item1, VItem* item2)
{
  item1->next = MemItemAddr(mem, item2);
}

#define MemItemNext(mem, item) \
  ( ItemNext(item) ?  MemItemPointer(mem, ItemNext(item)) : NULL )

// -------------------------------------------------------------
// Cons and rest
// -------------------------------------------------------------

// Adds an item to the front of a new list
// Allocs and returns the new list
VItem* MemCons(VMem* mem, VItem* item, VItem* list)
{
  // Alloc the new list item
  VItem* newList = MemAllocItem(mem);
  if (!newList) return NULL;
  ItemSetType(newList, TypeList);

  // Alloc the first item in the list
  VItem* newFirst = MemAllocItem(mem);
  if (!newFirst) return NULL;
  *newFirst = *item;

  // Set next of first
  VItem* first = MemItemFirst(mem, list);
  if (NULL == first)
    newFirst->next = 0;
  else
    newFirst->next = MemItemAddr(mem, first);
  
  // Set first of list
  MemItemSetFirst(mem, newList, newFirst);

  return newList;
}

// Allocs and returns a new list
VItem* MemRest(VMem* mem, VItem* list)
{
  VItem* next = NULL;

  // Get next of first
  VItem* first = MemItemFirst(mem, list);
  if (NULL != first)
  {
    next = MemItemNext(mem, first);
  }
  
  if (NULL == next)
  {
    // Return "nil"
    return NULL;
  }

  // Alloc new list item
  VItem* newList = MemAllocItem(mem);
  if (!newList) return NULL;
  ItemSetType(newList, TypeList);

  // Set first of new list
  MemItemSetFirst(mem, newList, next);

  return newList;
}

// -------------------------------------------------------------
// String items
// -------------------------------------------------------------

// Allocates a new item to hold the string
void MemItemSetString(VMem* mem, VItem* item, char* string)
{
  char* s = StrCopy(string);
  VItem* holder = MemAllocItem(mem);
  holder->string = s;
  ItemSetType(holder, TypeStringHolder);

  ItemSetType(item, TypeString);
  MemItemSetFirst(mem, item, holder);
}

char* MemItemString(VMem* mem, VItem* item)
{
  VItem* holder = MemItemFirst(mem, item);

  if (NULL == holder) return NULL;
  if (!IsTypeStringHolder(holder)) return NULL;

  return holder->string;
}

// -------------------------------------------------------------
// GC
// -------------------------------------------------------------

void MemMark(VMem* mem, VItem* item)
{
  while (item)
  {
    if (ItemGCMark(item))
    {
      return;
    }

    //printf("mark item\n");
    ItemSetGCMark(item, 1);

    if (IsTypeList(item) || IsTypeFun(item) || IsTypeString(item))
    {
      VItem* child = MemItemFirst(mem, item);
      MemMark(mem, child);
    }

    item = MemItemNext(mem, item);
  }
}

void MemSweep(VMem* mem)
{
  VItem* item = mem->start;

  while ((void*)item < mem->end)
  {
    if (ItemGCMark(item))
    {
      //printf("unmark\n");
      ItemSetGCMark(item, 0);
    }
    else
    {
      //printf("dealloc\n");
      MemDeallocItem(mem, item);
    }

    ++ item;
  }
}

// -------------------------------------------------------------
// Print items
// -------------------------------------------------------------

void MemPrintList(VMem* mem, VItem* first);

void MemPrintItem(VMem* mem, VItem* item)
{
  //printf("[T%i]", ItemType(item));
  if (IsTypeList(item))
    MemPrintList(mem, item);
  else if (IsTypeIntNum(item))
    printf("%li", item->intNum);
  else if (IsTypeDecNum(item))
    printf("%g", item->decNum);
  else if (IsTypePrimFun(item))
    printf("P%li", item->intNum);
  else if (IsTypeSymbol(item))
    printf("S%li", item->intNum);
  else if (IsTypeString(item))
    printf("'%s'", (char*)MemItemString(mem, item));
  else if (IsTypeFun(item))
  {
    printf("[FUN] ");
    MemPrintList(mem, item);
  }
}

void MemPrintList(VMem* mem, VItem* list)
{
  PrintChar('(');

  int printSpace = FALSE;
  VItem* item = MemItemFirst(mem, list);

  while (item)
  {
    if (printSpace) printf(" ");
    MemPrintItem(mem, item);
    item = MemItemNext(mem, item);
    printSpace = TRUE;
  }

  PrintChar(')');
}

void MemPrintArray(VMem* mem, VItem* array, int numItems)
{
  for (int i = 0; i < numItems; ++ i)
  {
    MemPrintItem(mem, &(array[i]));
    PrintChar(' ');
  }
}
