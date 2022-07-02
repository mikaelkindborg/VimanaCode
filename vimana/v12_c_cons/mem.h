/*
File: mem.h
Author: Mikael Kindborg (mikael@kindborg.com)

Memory manager for Lisp-style linked lists.

See item.h for an explanation of memory layout.
*/

// -------------------------------------------------------------
// VMem struct
// -------------------------------------------------------------

typedef struct __VMem
{
  VAddr  firstFree;
  VAddr  maxNumItems;
  VItem* start;
  VItem* end;
  #ifdef TRACK_MEMORY_USAGE
  int    allocCounter;
  #endif
}
VMem;

// -------------------------------------------------------------
// Item access
// -------------------------------------------------------------

#define MemStart(mem) ((mem)->start)

#define AddrToPtr(addr, start) ((start) + ((addr) - 1))
#define PtrToAddr(ptr, start) ((ptr - start) + 1)

#define MemGet(mem, addr) (AddrToPtr(addr, MemStart(mem)))
#define MemGetAddr(mem, ptr) (PtrToAddr(ptr, MemStart(mem)))

#define MemGetFirst(mem, item) \
  (ItemGetFirst(item) ? MemGet(mem, ItemGetFirst(item)) : NULL)

#define MemGetNext(mem, item) \
  (ItemGetNext(item) ? MemGet(mem, ItemGetNext(item)) : NULL)

void MemSetFirst(VMem* mem, VItem* item, VItem* first)
{
  ItemSetFirst(item, MemGetAddr(mem, first));
}

void MemSetNext(VMem* mem, VItem* item, VItem* next)
{
  ItemSetNext(item, MemGetAddr(mem, next));
}

// -------------------------------------------------------------
// Initialize
// -------------------------------------------------------------

// Return number of bytes needed to hold header struct plus items
int MemGetByteSize(int numItems)
{
  // Size of VMem header plus item memory space in bytes
  int byteSize = sizeof(VMem) + (numItems * sizeof(VItem));
  return byteSize;
}

void MemInit(VMem* mem, int numItems)
{
  mem->firstFree = 0;
  mem->maxNumItems = numItems;
  mem->start = (VItem*) (BytePtr(mem) + sizeof(VMem));
  mem->end = mem->start;
  #ifdef TRACK_MEMORY_USAGE
  mem->allocCounter = 0;
  #endif
}

#ifdef TRACK_MEMORY_USAGE
void MemPrintAllocCounter(VMem* mem)
{
  Print("MemAllocCounter: "); 
  PrintIntNum(mem->allocCounter); 
  PrintNewLine();
}
#endif

// -------------------------------------------------------------
// Alloc and dealloc
// -------------------------------------------------------------

// Allocate an item
VItem* MemAlloc(VMem* mem)
{
  VItem* item;
  VAddr  addr;

  if (mem->firstFree)
  {
    // Allocate item from the freelist
    PrintLine("Allocate from freelist");
    addr = mem->firstFree;
    item = MemGet(mem, addr);
    mem->firstFree = ItemGetNext(item);
  }
  else
  {
    // Allocate item from free memory
    VAddr freeAddr = mem->end - mem->start;
    //printf("memoryUsed: %lu\n", freeAddr);
    if (freeAddr < mem->maxNumItems)
    {
      PrintLine("Allocate from free space");
      addr = freeAddr;
      item = mem->end;
      ++ mem->end;
    }
    else
    {
      #ifdef DEBUG
        PrintLine("[GURU_MEDITATION] ALLOC_OUT_OF_ITEM_MEMORY");
        return NULL;
      #else
        GURU_MEDITATION(ALLOC_OUT_OF_ITEM_MEMORY);
      #endif
    }
  }

  #ifdef TRACK_MEMORY_USAGE
    ++ mem->allocCounter;
    MemPrintAllocCounter(mem);
  #endif

  // Init and set default type
  ItemInit(item);
  ItemSetType(item, TypeIntNum);

  return item;
}

void MemDeallocItem(VMem* mem, VItem* item)
{
  if (IsTypeNone(item))
  {
    printf("MemDeallocItem: IsTypeNone\n");
    return;
  }

  PrintLine("Dealloc item");

  #ifdef TRACK_MEMORY_USAGE
    -- mem->allocCounter;
    MemPrintAllocCounter(mem);
  #endif

  if (IsTypeBuffer(item))
  {
    PrintLine("FREE BUFFER");
    if (item->ptr) SysFree(item->ptr);
  }

  ItemSetType(item, TypeNone);
  ItemSetNext(item, mem->firstFree);
  mem->firstFree = PtrToAddr(item, mem->start);

  printf("firstFree: %lu\n", mem->firstFree);
}

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
// Ownership of bufferPtr goes to the memory manager
// bufferPtr must be allocated with SysAlloc (malloc)
//
VItem* MemAllocHandle(VMem* mem, void* bufferPtr, VUInt type)
{
  VItem* item = MemAlloc(mem);
  ItemSetType(item, type);

  VItem* buffer = MemAlloc(mem);
  ItemSetType(buffer, TypeBuffer);

  buffer->ptr = bufferPtr;
  
  MemSetFirst(mem, item, buffer);

  return item;
}

// Returns the pointer of the buffer the item refers to.
// TODO: Use GURU_MEDITATION instead of returning NULL?
void* MemGetHandlePtr(VMem* mem, VItem* item)
{
  if (!(IsTypeString(item) || IsTypeHandle(item))) return NULL;

  VItem* buffer = MemGetFirst(mem, item);

  if (NULL == buffer) return NULL;
  if (!IsTypeBuffer(buffer)) return NULL;

  return buffer->ptr;
}

// -------------------------------------------------------------
// GC
// -------------------------------------------------------------

void MemMark(VMem* mem, VItem* item)
{
  while (item)
  {
    if (ItemGetGCMark(item))
    {
      PrintLine("ALREADY MARKED");
      return;
    }

    //Print("MARK ITEM: "); MemPrintItem(mem, item); PrintNewLine();
    ItemSetGCMark(item, 1);

    // Mark children
    if (!IsTypeAtomic(item))
    {
      VItem* child = MemGetFirst(mem, item);
      MemMark(mem, child);
    }

    item = MemGetNext(mem, item);
  }
}

void MemSweep(VMem* mem)
{
  VItem* item = mem->start;

  while (item < mem->end)
  {
    if (ItemGetGCMark(item))
    {
      PrintLine("MemSweep unmark");
      ItemSetGCMark(item, 0);
    }
    else
    {
      PrintLine("MemSweep dealloc");
      MemDeallocItem(mem, item);
    }

    ++ item;
  }
}
