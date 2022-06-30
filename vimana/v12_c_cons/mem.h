/*
File: itemMemory.h
Author: Mikael Kindborg (mikael@kindborg.com)

Memory manager for Lisp-style linked lists.

See item.h for an explanation of memory layout.
*/

// -------------------------------------------------------------
// VItemMemory struct
// -------------------------------------------------------------

typedef struct __VItemMemory
{
  VAddr firstFree;
  VAddr size;
  void* end;
  void* start;
#ifdef TRACK_MEMORY_USAGE
  int   allocCounter;
#endif
}
VItemMemory;

// TODO fix addressing
#define ItemAddrShift 3 // 64 bit machines
#define ItemMemoryAddrToPtr(itemMemory, addr) \
  ((VItem*) ((itemMemory)->start + ((addr) << ItemAddrShift))) 
// TODO Fix addressing (+ 1)
MIKI
#define ItemMemoryPtrToAddr(itemMemory, itemPtr) \
  ( (NULL != (itemPtr)) ? ( ((void*)(itemPtr)) - (itemMemory)->start ) + 1 : 0 )

// Return the number of bytes needed to hold VItemMemory header plus numItems
int ItemMemoryByteSize(int numItems)
{
  // Size of VItemMemory header plus item memory space in bytes
  int byteSize = sizeof(VItemMemory) + (numItems * sizeof(VItem));
  return byteSize;
}

void ItemMemoryInit(VItemMemory* itemMemory, int numItems)
{
  VAddr byteSize = ItemMemoryByteSize(numItems);

  itemMemory->firstFree = 0;
  itemMemory->size = byteSize - sizeof(VItemMemory);
  itemMemory->start = itemMemory + sizeof(VItemMemory); // TODO offset start?
  itemMemory->end = itemMemory->start;
#ifdef TRACK_MEMORY_USAGE
  itemMemory->allocCounter = 0;
#endif
}

#ifdef TRACK_MEMORY_USAGE
  void MPrintAllocCounter(VItemMemory* itemMemory)
  {
    Print("MemAllocCounter: "); PrintIntNum(itemMemory->allocCounter); PrintNewLine();
  }
#endif

// -------------------------------------------------------------
// Alloc and dealloc
// -------------------------------------------------------------

// TODO: Massive renaming
MIKI
VItem* ItemMemoryAlloc(VItemMemory* itemMemory)
{
  VItem* item;

  if (itemMemory->firstFree)
  {
    //printf("Free item available\n");
    item = MemItemPointer(itemMemory, itemMemory->firstFree);
    itemMemory->firstFree = ItemNextAddr(item);
  }
  else
  {
    VAddr memoryUsed = itemMemory->end - itemMemory->start;
    //printf("memoryUsed: %i\n", memoryUsed);
    //printf("itemMemory->size: %i\n", itemMemory->size);
    if (!(memoryUsed < (itemMemory->size + sizeof(VItem))))
    {
      // For debugging
      // TODO add debug flag
      printf("[GURU_MEDITATION: -1] MEM OUT OF MEMORY\n");
      return NULL;
      // TODO if not debug raise error
      // GURU(MEM_OUT_OF_MEMORY);
    }
    //printf("Free space available\n");
    item = itemMemory->end;
    itemMemory->end += sizeof(VItem);
  }

  ItemInit(item);

#ifdef TRACK_MEMORY_USAGE
  ++ (itemMemory->allocCounter);
#endif

  return item;
}

void MemDeallocItem(VItemMemory* itemMemory, VItem* item)
{
  if (IsTypeNone(item))
  {
    //printf("MemDeallocItem: IsTypeNone\n");
    return;
  }

#ifdef TRACK_MEMORY_USAGE
  -- (itemMemory->allocCounter);
#endif

  if (IsTypeBufferPtr(item))
  {
    //PrintLine("FREE BUFFERPTR");
    if (item->ptr) SysFree(item->ptr);
  }

  ItemSetType(item, TypeNone);
  
//TODO ->next
  MIKI
  item->next = itemMemory->firstFree;

  itemMemory->firstFree = MemItemAddr(itemMemory, item);
}

// -------------------------------------------------------------
// Item access
// -------------------------------------------------------------

void MemItemSetFirst(VItemMemory* itemMemory, VItem* list, VItem* first)
{
  list->addr = MemItemAddr(itemMemory, first);
}

#define MemItemFirst(itemMemory, list) \
  ( ((list) && (list)->addr) ?  MemItemPointer(itemMemory, (list)->addr) : NULL )

void MemItemSetNext(VItemMemory* itemMemory, VItem* item1, VItem* item2)
{
  ItemSetNextAddr(item1, MemItemAddr(itemMemory, item2));
}

#define MemItemNext(itemMemory, item) \
  ( ItemNextAddr(item) ?  MemItemPointer(itemMemory, ItemNextAddr(item)) : NULL )

// -------------------------------------------------------------
// Cons and rest
// -------------------------------------------------------------
/*
// Adds an item to the front of a new list
// Allocs and returns the new list
VItem* MemCons(VItemMemory* itemMemory, VItem* item, VItem* list)
{
  // Alloc the new list item
  VItem* newList = MemAllocItem(itemMemory);
  if (!newList) return NULL;
  ItemSetType(newList, TypeList);

  // Alloc the first item in the list
  VItem* newFirst = MemAllocItem(itemMemory);
  if (!newFirst) return NULL;
  *newFirst = *item;

  // Set next of first
  VItem* first = MemItemFirst(itemMemory, list);
  if (NULL == first)
    newFirst->next = 0;
  else
    newFirst->next = MemItemAddr(itemMemory, first);
  
  // Set first of list
  MemItemSetFirst(itemMemory, newList, newFirst);

  return newList;
}

// Allocs and returns a new list
VItem* MemRest(VItemMemory* itemMemory, VItem* list)
{
  VItem* next = NULL;

  // Get next of first
  VItem* first = MemItemFirst(itemMemory, list);
  if (NULL != first)
  {
    next = MemItemNext(itemMemory, first);
  }
  
  if (NULL == next)
  {
    // Return "nil"
    return NULL;
  }

  // Alloc new list item
  VItem* newList = MemAllocItem(itemMemory);
  if (!newList) return NULL;
  ItemSetType(newList, TypeList);

  // Set first of new list
  MemItemSetFirst(itemMemory, newList, next);

  return newList;
}
*/

// -------------------------------------------------------------
// Buffer items
// -------------------------------------------------------------

// Allocate two new items: one that can be copied and one that 
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
VItem* MemAllocBufferItem(VItemMemory* itemMemory, void* bufferPtr)
{
  //PrintLine("MemAllocBufferItem");
  VItem* item = MemAllocItem(itemMemory);
  VItem* bufferItem = MemAllocItem(itemMemory);
  
  bufferItem->ptr = bufferPtr;
  ItemSetType(bufferItem, TypeBufferPtr);
  MemItemSetFirst(itemMemory, item, bufferItem);

  return item;
}

// Returns the pointer of the buffer the item refers to.
void* MemBufferItemPtr(VItemMemory* itemMemory, VItem* item)
{
  VItem* bufferItem = MemItemFirst(itemMemory, item);

  if (NULL == bufferItem) return NULL;
  if (!IsTypeBufferPtr(bufferItem)) return NULL;

  return bufferItem->ptr;
}

// -------------------------------------------------------------
// GC
// -------------------------------------------------------------

void MemPrintItem(VItemMemory* itemMemory, VItem* item);

void MemMark(VItemMemory* itemMemory, VItem* item)
{
  while (item)
  {
    if (ItemGCMark(item))
    {
      PrintLine("ALREADY MARKED");
      return;
    }

    //Print("MARK ITEM: "); MemPrintItem(itemMemory, item); PrintNewLine();
    ItemSetGCMark(item, 1);

    // Mark children
    if (!IsTypeAtomic(item))
    {
      VItem* child = MemItemFirst(itemMemory, item);
      MemMark(itemMemory, child);
    }

    item = MemItemNext(itemMemory, item);
  }
}

void MemSweep(VItemMemory* itemMemory)
{
  VItem* item = itemMemory->start;

  while ((void*)item < itemMemory->end)
  {
    if (ItemGCMark(item))
    {
      //PrintLine("MemSweep unmark");
      ItemSetGCMark(item, 0);
    }
    else
    {
      //PrintLine("MemSweep dealloc");
      MemDeallocItem(itemMemory, item);
    }

    ++ item;
  }
}

// -------------------------------------------------------------
// Print items
// -------------------------------------------------------------

void MemPrintList(VItemMemory* itemMemory, VItem* first);

void MemPrintItem(VItemMemory* itemMemory, VItem* item)
{
  //printf("[T%i]", ItemType(item));
  if (IsTypeNone(item))
    Print("None");
  else if (IsTypeList(item))
    MemPrintList(itemMemory, item);
  else if (IsTypeIntNum(item))
    printf("%li", item->intNum);
  else if (IsTypeDecNum(item))
    printf("%g", item->decNum);
  else if (IsTypePrimFun(item))
    printf("P%li", item->intNum);
  else if (IsTypeSymbol(item))
    printf("S%li", item->intNum);
  else if (IsTypeString(item))
    printf("'%s'", (char*)MemBufferItemPtr(itemMemory, item));
  else if (IsTypeFun(item))
  {
    printf("[FUN] ");
    MemPrintList(itemMemory, item);
  }
}

void MemPrintList(VItemMemory* itemMemory, VItem* list)
{
  PrintChar('(');

  int printSpace = FALSE;
  VItem* item = MemItemFirst(itemMemory, list);

  while (item)
  {
    if (printSpace) PrintChar(' ');
    MemPrintItem(itemMemory, item);
    item = MemItemNext(itemMemory, item);
    printSpace = TRUE;
  }

  PrintChar(')');
}

void MemPrintArray(VItemMemory* itemMemory, VItem* array, int numItems)
{
  for (int i = 0; i < numItems; ++ i)
  {
    MemPrintItem(itemMemory, &(array[i]));
    PrintChar(' ');
  }
}
