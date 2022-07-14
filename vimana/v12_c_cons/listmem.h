/*
File: listmem.h
Author: Mikael Kindborg (mikael@kindborg.com)

Memory manager for Lisp-style linked lists.

See item.h for an explanation of memory layout.
*/

// -------------------------------------------------------------
// VListMemory struct
// -------------------------------------------------------------

typedef struct __VListMemory
{
  VByte* start;     // Start of memory block
  VByte* nextFree;  // First free item in memory block
  VAddr  firstFree; // Address to first item in freelist
  VAddr  size;      // Total size of item memory
  #ifdef TRACK_MEMORY_USAGE
  int    allocCounter;
  #endif
}
VListMemory;

// -------------------------------------------------------------
// Item access
// -------------------------------------------------------------

#define ListMemStart(mem) ((mem)->start)

//#define AddrToPtr(addr, start) (((start) + (addr)) - 1)
//#define PtrToAddr(ptr, start) ((BytePtr(ptr) - (start)) + 1)

#define AddrToPtr(addr, start) ((start) + (addr))
#define PtrToAddr(ptr, start) (BytePtr(ptr) - (start))

#define ListMemGet(mem, addr) VItemPtr(AddrToPtr(addr, ListMemStart(mem)))
#define ListMemGetAddr(mem, ptr) (PtrToAddr(BytePtr(ptr), ListMemStart(mem)))

#define ListMemGetFirst(mem, item) \
  (ItemGetFirst(item) ? ListMemGet(mem, ItemGetFirst(item)) : NULL)

#define ListMemGetNext(mem, item) \
  (ItemGetNext(item) ? ListMemGet(mem, ItemGetNext(item)) : NULL)

void ListMemSetFirst(VListMemory* mem, VItem* item, VItem* first)
{
  ItemSetFirst(item, ListMemGetAddr(mem, first));
}

void ListMemSetNext(VListMemory* mem, VItem* item, VItem* next)
{
  ItemSetNext(item, ListMemGetAddr(mem, next));
}

// -------------------------------------------------------------
// Initialize
// -------------------------------------------------------------

// Return size of VListMemory header plus item memory space in bytes
int ListMemGetByteSize(int numItems)
{
  return sizeof(VListMemory) + (numItems * sizeof(VItem));
}

void ListMemInit(VListMemory* mem, int numItems)
{
  VAddr memByteSize = ListMemGetByteSize(numItems);

  mem->start = BytePtr(mem) + (sizeof(VListMemory));
  mem->nextFree = mem->start;
  mem->size = (memByteSize - sizeof(VListMemory));
  mem->firstFree = 0; // Freelist is empty

  #ifdef TRACK_MEMORY_USAGE
  mem->allocCounter = 0;
  #endif
}

#ifdef TRACK_MEMORY_USAGE
void ListMemPrintAllocCounter(VListMemory* mem)
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
VItem* ListMemAlloc(VListMemory* mem)
{
  VItem* item;
  VAddr  addr;

  if (mem->firstFree)
  {
    // ALLOCATE FROM FREELIST

    addr = mem->firstFree;
    item = ListMemGet(mem, addr);
    mem->firstFree = ItemGetNext(item);
  }
  else
  {
    // ALLOCATE FROM UNUSED MEMORY

    if (mem->nextFree < mem->start + mem->size)
    {
      // We have a bit of truble here, because the first item will have addr 0,
      // and address zero is the list terminator. This means you won't be able 
      // to reference the first item allocated in the next field.
      // This can be fixed, but I will leave it as is for now.
      item = VItemPtr(mem->nextFree);
      mem->nextFree += sizeof(VItem);
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

void ListMemDeallocItem(VListMemory* mem, VItem* item)
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
VItem* ListMemAllocHandle(VListMemory* mem, void* bufferPtr, VType type)
{
  VItem* item = ListMemAlloc(mem);
  ItemSetType(item, type);

  VItem* buffer = ListMemAlloc(mem);
  ItemSetType(buffer, TypeBuffer);

  buffer->ptr = bufferPtr;
  
  ListMemSetFirst(mem, item, buffer);

  return item;
}

// Returns the pointer of the buffer the item refers to.
// TODO: Use GURU_MEDITATION instead of returning NULL?
void* ListMemGetHandlePtr(VListMemory* mem, VItem* item)
{
  if (!(IsTypeString(item) || IsTypeHandle(item))) return NULL;

  VItem* buffer = ListMemGetFirst(mem, item);

  if (NULL == buffer) return NULL;
  if (!IsTypeBuffer(buffer)) return NULL;

  return buffer->ptr;
}

// -------------------------------------------------------------
// Garbage collection
// -------------------------------------------------------------

void ListMemMark(VListMemory* mem, VItem* item)
{
  while (item)
  {
    if (ItemGetGCMark(item))
    {
      //PrintLine("ALREADY MARKED");
      return;
    }

    //Print("MARK ITEM: "); ListMemPrintItem(mem, item); PrintNewLine();
    ItemGCMarkSet(item);

    // Mark children
    if (!IsTypeAtomic(item))
    {
      VItem* child = ListMemGetFirst(mem, item);
      ListMemMark(mem, child);
    }

    item = ListMemGetNext(mem, item);
  }
}

void ListMemSweep(VListMemory* mem)
{
  VByte* ptr = mem->start;

  while (ptr < mem->nextFree)
  {
    if (ItemGetGCMark(VItemPtr(ptr)))
    {
      //PrintLine("MemSweep unmark");
      ItemGCMarkUnset(VItemPtr(ptr));
    }
    else
    {
      //PrintLine("MemSweep dealloc");
      ListMemDeallocItem(mem, VItemPtr(ptr));
    }

    ptr += sizeof(VItem);
  }
}
