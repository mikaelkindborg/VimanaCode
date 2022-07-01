/*
File: cellmemory.h
Author: Mikael Kindborg (mikael@kindborg.com)

Memory manager for Lisp-style linked lists.

See item.h for an explanation of memory layout.
*/

// -------------------------------------------------------------
// VCellMemory struct
// -------------------------------------------------------------

typedef struct __VCellMemory
{
  VAddr firstFree;
  VAddr size;
  VAddr cellSize;
  void* end;
  void* start;
#ifdef TRACK_MEMORY_USAGE
  int   allocCounter;
#endif
}
VCellMemory;

// -------------------------------------------------------------
// Initialize
// -------------------------------------------------------------

// Return number of bytes needed to hold header struct plus items
int CellMemoryByteSize(int numItems)
{
  // Size of VCellMemory header plus item memory space in bytes
  int byteSize = sizeof(VCellMemory) + (numItems * sizeof(VItem));
  return byteSize;
}

void CellMemoryInit(VCellMemory* cellMemory, int numItems)
{
  VAddr byteSize = CellMemoryByteSize(numItems);

  cellMemory->cellSize = sizeof(VItem);
  cellMemory->firstFree = 0;
  cellMemory->size = byteSize - sizeof(VCellMemory);
  cellMemory->start = cellMemory + sizeof(VCellMemory);
  cellMemory->end = cellMemory->start;
#ifdef TRACK_MEMORY_USAGE
  cellMemory->allocCounter = 0;
#endif
}

#ifdef TRACK_MEMORY_USAGE
  void CellMemoryPrintAllocCounter(VCellMemory* cellMemory)
  {
    Print("CellMemoryAllocCounter: "); 
    PrintIntNum(cellMemory->allocCounter); 
    PrintNewLine();
  }
#endif

/*
TODO: REMOVE OLD CODE
// Get pointer to first item
#define CellMemoryGetFirst(cellMemory, list) \
  ( ((list) && (list)->addr) ? \
    CellMemoryAddrToPtr(cellMemory, (list)->addr) : NULL )

// Get pointer to next item
#define CellMemoryGetNext(cellMemory, item) \
  ( ItemNext(item) ?  CellMemoryAddrToPtr(cellMemory, ItemNext(item)) : NULL )
*/

// -------------------------------------------------------------
// Alloc and dealloc
// -------------------------------------------------------------

// TODO: Massive renaming ahead

// Allocate an item
VItem* CellMemoryAllocItem(VCellMemory* cellMemory)
{
  VItem* item;

  if (cellMemory->firstFree)
  {
    //printf("Free item is available\n");
    item = ItemAddrToPtr(cellMemory->firstFree, cellMemory->start);
    cellMemory->firstFree = ItemGetNext(item);
  }
  else
  {
    VAddr memoryUsed = cellMemory->end - cellMemory->start;
    //printf("memoryUsed: %i\n", memoryUsed);
    //printf("cellMemory->size: %i\n", cellMemory->size);
    if (! (memoryUsed <= (cellMemory->size + sizeof(VItem))) )
    {
      // For debugging
      // TODO add debug flag
      printf("[GURU_MEDITATION: -1] MEM OUT OF MEMORY\n");
      return NULL;
      // TODO if not debug raise error
      // GURU_MEDITATION(ALLOC_OUT_OF_ITEM_MEMORY);
    }
    //printf("Free space available\n");
    item = cellMemory->end;
    cellMemory->end += sizeof(VItem);
  }

  ItemInit(item);

#ifdef TRACK_MEMORY_USAGE
  ++ (cellMemory->allocCounter);
#endif

  return item;
}

void MemDeallocItem(VCellMemory* cellMemory, VItem* item)
{
  if (IsTypeNone(item))
  {
    //printf("MemDeallocItem: IsTypeNone\n");
    return;
  }

#ifdef TRACK_MEMORY_USAGE
  -- (cellMemory->allocCounter);
#endif

  if (IsTypeBufferPtr(item))
  {
    //PrintLine("FREE BUFFERPTR");
    if (item->ptr) SysFree(item->ptr);
  }

  ItemSetType(item, TypeNone);
  
//TODO ->next
  MIKI
  item->next = cellMemory->firstFree;

  cellMemory->firstFree = MemItemAddr(cellMemory, item);
}

// -------------------------------------------------------------
// Cons and rest
// -------------------------------------------------------------
/*
// Adds an item to the front of a new list
// Allocs and returns the new list
VItem* MemCons(VCellMemory* cellMemory, VItem* item, VItem* list)
{
  // Alloc the new list item
  VItem* newList = MemAllocItem(cellMemory);
  if (!newList) return NULL;
  ItemSetType(newList, TypeList);

  // Alloc the first item in the list
  VItem* newFirst = MemAllocItem(cellMemory);
  if (!newFirst) return NULL;
  *newFirst = *item;

  // Set next of first
  VItem* first = MemItemFirst(cellMemory, list);
  if (NULL == first)
    newFirst->next = 0;
  else
    newFirst->next = MemItemAddr(cellMemory, first);
  
  // Set first of list
  MemItemSetFirst(cellMemory, newList, newFirst);

  return newList;
}

// Allocs and returns a new list
VItem* MemRest(VCellMemory* cellMemory, VItem* list)
{
  VItem* next = NULL;

  // Get next of first
  VItem* first = MemItemFirst(cellMemory, list);
  if (NULL != first)
  {
    next = MemItemNext(cellMemory, first);
  }
  
  if (NULL == next)
  {
    // Return "nil"
    return NULL;
  }

  // Alloc new list item
  VItem* newList = MemAllocItem(cellMemory);
  if (!newList) return NULL;
  ItemSetType(newList, TypeList);

  // Set first of new list
  MemItemSetFirst(cellMemory, newList, next);

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
VItem* MemAllocBufferItem(VCellMemory* cellMemory, void* bufferPtr)
{
  //PrintLine("MemAllocBufferItem");
  VItem* item = MemAllocItem(cellMemory);
  VItem* bufferItem = MemAllocItem(cellMemory);
  
  bufferItem->ptr = bufferPtr;
  ItemSetType(bufferItem, TypeBufferPtr);
  MemItemSetFirst(cellMemory, item, bufferItem);

  return item;
}

// Returns the pointer of the buffer the item refers to.
void* CellMemoryGetBufferPtr(VCellMemory* cellMemory, VItem* item)
{
  VItem* bufferItem = CellMemoryGetFirst(cellMemory, item);

  if (NULL == bufferItem) return NULL;
  if (!IsTypeBufferPtr(bufferItem)) return NULL;

  return bufferItem->ptr;
}

// -------------------------------------------------------------
// GC
// -------------------------------------------------------------

void MemPrintItem(VCellMemory* cellMemory, VItem* item);

void MemMark(VCellMemory* cellMemory, VItem* item)
{
  while (item)
  {
    if (ItemGCMark(item))
    {
      PrintLine("ALREADY MARKED");
      return;
    }

    //Print("MARK ITEM: "); MemPrintItem(cellMemory, item); PrintNewLine();
    ItemSetGCMark(item, 1);

    // Mark children
    if (!IsTypeAtomic(item))
    {
      VItem* child = CellMemoryGetFirst(cellMemory, item);
      MemMark(cellMemory, child);
    }

    item = MemItemNext(cellMemory, item);
  }
}

void MemSweep(VCellMemory* cellMemory)
{
  VItem* item = cellMemory->start;

  while ((void*)item < cellMemory->end)
  {
    if (ItemGCMark(item))
    {
      //PrintLine("MemSweep unmark");
      ItemSetGCMark(item, 0);
    }
    else
    {
      //PrintLine("MemSweep dealloc");
      MemDeallocItem(cellMemory, item);
    }

    ++ item;
  }
}
