/*
File: mem.h
Author: Mikael Kindborg (mikael@kindborg.com)

Memory manager for List-style linked items.

See item.h for an explanation of memory layout.
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
#ifdef TRACK_MEMORY_USAGE
  int   allocCounter;
#endif
}
VMem;

#define MemItemPointer(mem, addr) ( (VItem*) ((mem)->start + (addr) - 1) ) 

// Return the number of bytes needed to hold VMem header plus numItems
int MemByteSize(int numItems)
{
  // Size of VMem header plus item memory in bytes
  int memByteSize = sizeof(VMem) + (numItems * sizeof(VItem));
  return memByteSize;
}

void MemInit(VMem* mem, int numItems)
{
  VAddr memByteSize = MemByteSize(numItems);

  mem->firstFree = 0;
  mem->size = memByteSize - sizeof(VMem);
  mem->start = mem + sizeof(VMem);
  mem->end = mem->start;
#ifdef TRACK_MEMORY_USAGE
  mem->allocCounter = 0;
#endif
}

#ifdef TRACK_MEMORY_USAGE
  void MemPrintAllocCounter(VMem* mem)
  {
    Print("MemAllocCounter: "); PrintIntNum(mem->allocCounter); PrintNewLine();
  }
#endif

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
      // For debugging
      printf("[GURU_MEDITATION: -1] MEM OUT OF MEMORY\n");
      return NULL;
      // GURU(MEM_OUT_OF_MEMORY);
    }
    //printf("Free space available\n");
    item = mem->end;
    mem->end += sizeof(VItem);
  }

  ItemInit(item);

#ifdef TRACK_MEMORY_USAGE
  ++ (mem->allocCounter);
#endif

  return item;
}

void MemDeallocItem(VMem* mem, VItem* item)
{
  if (IsTypeNone(item))
  {
    //printf("MemDeallocItem: IsTypeNone\n");
    return;
  }

#ifdef TRACK_MEMORY_USAGE
  -- (mem->allocCounter);
#endif

  if (IsTypeBufferPtr(item))
  {
    //PrintLine("FREE BUFFERPTR");
    if (item->ptr) SysFree(item->ptr);
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
  ( ((list) && (list)->addr) ?  MemItemPointer(mem, (list)->addr) : NULL )

void MemItemSetNext(VMem* mem, VItem* item1, VItem* item2)
{
  item1->next = MemItemAddr(mem, item2);
}

#define MemItemNext(mem, item) \
  ( ItemNext(item) ?  MemItemPointer(mem, ItemNext(item)) : NULL )

// -------------------------------------------------------------
// Cons and rest
// -------------------------------------------------------------
/*
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
*/

// -------------------------------------------------------------
// Buffer items
// -------------------------------------------------------------

// Allocates two new items: one that can be copied and one that 
// points to the memory buffer. The memory buffer must be allocated
// with malloc() or a similar function. The caller must set
// the type of the returned item.
//
// There must be only one instance of the item that points to 
// allocated memory, and this item must NOT be shared; that is,
// it must NOT be present on the data stack or used for variables.
// If the raw pointer would be refereced from many items, the
// garbage collector would not be able to tell if it is deallocated.
//
VItem* MemAllocBufferItem(VMem* mem, void* bufferPtr)
{
  //PrintLine("MemAllocBufferItem");
  VItem* item = MemAllocItem(mem);
  VItem* bufferItem = MemAllocItem(mem);
  
  bufferItem->ptr = bufferPtr;
  ItemSetType(bufferItem, TypeBufferPtr);
  MemItemSetFirst(mem, item, bufferItem);

  return item;
}

// Returns the pointer of the buffer the item refers to.
void* MemBufferItemPtr(VMem* mem, VItem* item)
{
  VItem* bufferItem = MemItemFirst(mem, item);

  if (NULL == bufferItem) return NULL;
  if (!IsTypeBufferPtr(bufferItem)) return NULL;

  return bufferItem->ptr;
}

// -------------------------------------------------------------
// GC
// -------------------------------------------------------------

void MemPrintItem(VMem* mem, VItem* item);

void MemMark(VMem* mem, VItem* item)
{
  while (item)
  {
    if (ItemGCMark(item))
    {
      PrintLine("ALREADY MARKED");
      return;
    }

    //Print("MARK ITEM: "); MemPrintItem(mem, item); PrintNewLine();
    ItemSetGCMark(item, 1);

    // Mark children
    if (!IsTypeAtomic(item))
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
      //PrintLine("MemSweep unmark");
      ItemSetGCMark(item, 0);
    }
    else
    {
      //PrintLine("MemSweep dealloc");
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
  if (IsTypeNone(item))
    Print("None");
  else if (IsTypeList(item))
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
    printf("'%s'", (char*)MemBufferItemPtr(mem, item));
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
    if (printSpace) PrintChar(' ');
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
