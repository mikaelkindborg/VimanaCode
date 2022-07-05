/*
File: mem.h
Author: Mikael Kindborg (mikael@kindborg.com)

Memory manager for Lisp-style linked lists.

See item.h for an explanation of memory layout.
*/

// -------------------------------------------------------------
// VItemMemory struct
// -------------------------------------------------------------

typedef struct __VItemMemory
{
  VByte* start;     // Start of item memory
  VByte* end;       // End of used memory (first free)
  VAddr  firstFree; // Address to first item in freelist
  VAddr  size;      // Total size of item memory
  #ifdef TRACK_MEMORY_USAGE
  int    allocCounter;
  #endif
}
VItemMemory;

// -------------------------------------------------------------
// Item access
// -------------------------------------------------------------

#define MemStart(mem) ((mem)->start)

//#define AddrToPtr(addr, start) (((start) + (addr)) - 1)
//#define PtrToAddr(ptr, start) ((BytePtr(ptr) - (start)) + 1)

#define AddrToPtr(addr, start) ((start) + (addr))
#define PtrToAddr(ptr, start) (BytePtr(ptr) - (start))

#define MemGet(mem, addr) VItemPtr(AddrToPtr(addr, MemStart(mem)))
#define MemGetAddr(mem, ptr) (PtrToAddr(BytePtr(ptr), MemStart(mem)))

#define MemGetFirst(mem, item) \
  (ItemGetFirst(item) ? MemGet(mem, ItemGetFirst(item)) : NULL)

#define MemGetNext(mem, item) \
  (ItemGetNext(item) ? MemGet(mem, ItemGetNext(item)) : NULL)

void MemSetFirst(VItemMemory* mem, VItem* item, VItem* first)
{
  ItemSetFirst(item, MemGetAddr(mem, first));
}

void MemSetNext(VItemMemory* mem, VItem* item, VItem* next)
{
  ItemSetNext(item, MemGetAddr(mem, next));
}

// -------------------------------------------------------------
// Initialize
// -------------------------------------------------------------

// Return number of bytes needed to hold header struct plus items
int MemGetByteSize(int numItems)
{
  // Size of VItemMemory header plus item memory space in bytes
  int byteSize = sizeof(VItemMemory) + (numItems * sizeof(VItem));
  return byteSize;
}

void MemInit(VItemMemory* mem, int numItems)
{
  VAddr memByteSize = MemGetByteSize(numItems);

  mem->start = BytePtr(mem) + (sizeof(VItemMemory));
  mem->end = mem->start; // End of used space
  mem->size = (memByteSize - sizeof(VItemMemory));
  mem->firstFree = 0; // Freelist is empty

  #ifdef TRACK_MEMORY_USAGE
  mem->allocCounter = 0;
  #endif
}

#ifdef TRACK_MEMORY_USAGE
void MemPrintAllocCounter(VItemMemory* mem)
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
VItem* MemAlloc(VItemMemory* mem)
{
  VItem* item;
  VAddr  addr;

  if (mem->firstFree)
  {
    // ALLOCATE FROM FREELIST

    addr = mem->firstFree;
    item = MemGet(mem, addr);
    mem->firstFree = ItemGetNext(item);
  }
  else
  {
    // ALLOCATE FROM UNUSED MEMORY

    if (mem->end < mem->start + mem->size)
    {
      // We have a bit of truble here, because the first item will have addr 0,
      // and address zero is the list terminator. This means you won't be able 
      // to reference the first item allocated in the next field.
      // This can be fixed, but I will leave it as is for now.
      item = VItemPtr(mem->end);
      mem->end += sizeof(VItem);
    }
    else
    {
      #ifdef DEBUG
        PrintLine("[GURU_MEDITATION] ALLOC_ITEM_OUT_OF_MEMORY");
        return NULL;
      #else
        GURU_MEDITATION(ALLOC_ITEM_OUT_OF_MEMORY);
      #endif
    }
  }

  #ifdef TRACK_MEMORY_USAGE
    ++ mem->allocCounter;
  #endif

  // Init and set default type
  ItemInit(item);
  ItemSetType(item, TypeIntNum);

  return item;
}

void MemDeallocItem(VItemMemory* mem, VItem* item)
{
  if (IsTypeNone(item)) return;

  #ifdef TRACK_MEMORY_USAGE
    -- mem->allocCounter;
  #endif

  if (IsTypeBuffer(item))
  {
    //PrintLine("FREE BUFFER");
    if (ItemGetPtr(item)) SysFree(ItemGetPtr(item));
  }

  ItemSetType(item, TypeNone);
  ItemSetNext(item, mem->firstFree);
  mem->firstFree = PtrToAddr(item, mem->start);

  //printf("mem->firstFree: %lu\n", mem->firstFree);
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
VItem* MemAllocHandle(VItemMemory* mem, void* bufferPtr, VType type)
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
void* MemGetHandlePtr(VItemMemory* mem, VItem* item)
{
  if (!(IsTypeString(item) || IsTypeHandle(item))) return NULL;

  VItem* buffer = MemGetFirst(mem, item);

  if (NULL == buffer) return NULL;
  if (!IsTypeBuffer(buffer)) return NULL;

  return buffer->ptr;
}

// -------------------------------------------------------------
// Garbage collection
// -------------------------------------------------------------

void MemMark(VItemMemory* mem, VItem* item)
{
  while (item)
  {
    if (ItemGetGCMark(item))
    {
      //PrintLine("ALREADY MARKED");
      return;
    }

    //Print("MARK ITEM: "); MemPrintItem(mem, item); PrintNewLine();
    ItemGCMarkSet(item);

    // Mark children
    if (!IsTypeAtomic(item))
    {
      VItem* child = MemGetFirst(mem, item);
      MemMark(mem, child);
    }

    item = MemGetNext(mem, item);
  }
}

void MemSweep(VItemMemory* mem)
{
  VByte* ptr = mem->start;

  while (ptr < mem->end)
  {
    if (ItemGetGCMark(VItemPtr(ptr)))
    {
      //PrintLine("MemSweep unmark");
      ItemGCMarkUnset(VItemPtr(ptr));
    }
    else
    {
      //PrintLine("MemSweep dealloc");
      MemDeallocItem(mem, VItemPtr(ptr));
    }

    ptr += sizeof(VItem);
  }
}
