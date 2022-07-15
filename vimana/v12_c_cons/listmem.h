/*
File: listmem.h
Author: Mikael Kindborg (mikael@kindborg.com)

Memory manager for Lisp-style linked lists.

See item.h for an explanation of memory layout.
*/

// -------------------------------------------------------------
// VListMemory struct
// -------------------------------------------------------------

// Addresses in list memory are array indexes (VAddr)
// First item in the array reprecents the nil value

typedef struct __VListMemory
{
  //VByte* start;     // Start of memory block
  //VByte* nextFree;  // First free item in memory block
  VByte* start;     // Start of memory block
  VAddr  addrNextFree;  // First free item in memory block
  VAddr  addrFirstFree; // First item in freelist
  VAddr  addrEnd;   // End of memory
  #ifdef TRACK_MEMORY_USAGE
  int    allocCounter;
  #endif
}
VListMemory;

// -------------------------------------------------------------
// Item access
// -------------------------------------------------------------

#define ListMemStart(mem) ((mem)->start)

#define AddrToPtr(addr, start) (((start) + (addr)) - 1)
#define PtrToAddr(ptr, start) ((BytePtr(ptr) - (start)) + 1)

// Address is an index
//#define AddrToPtr(addr, start) ((start) + ((addr) << 4)) //* sizeof(VItem)))
//#define PtrToAddr(ptr, start) ((BytePtr(ptr) - (start)) >> 4) // sizeof(VItem))

#define ListMemGet(mem, addr) VItemPtr(AddrToPtr(addr, ListMemStart(mem)))
#define ListMemGetAddr(mem, ptr) PtrToAddr(ptr, ListMemStart(mem))

//#define ListMemGetFirst(mem, item) \
//  (ItemGetFirst(item) ? ListMemGet(mem, ItemGetFirst(item)) : NULL)
//#define ListMemGetNext(mem, item) \
//  (ItemGetNext(item) ? ListMemGet(mem, ItemGetNext(item)) : NULL)

#define ListMemGetFirst(mem, item) ListMemGet(mem, ItemGetFirst(item))
#define ListMemGetNext(mem, item)  ListMemGet(mem, ItemGetNext(item))

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
// One extra item is reserved for the nil value
int ListMemByteSize(int numItems)
{
  return sizeof(VListMemory) + ((1 + numItems) * sizeof(VItem));
}

void ListMemInit(VListMemory* mem, int numItems)
{
  VAddr memByteSize = ListMemByteSize(numItems);

  mem->start = BytePtr(mem) + sizeof(VListMemory);

  // Pointer address that represents nil
  GlobalNil = mem->start - 1;

  mem->addrNextFree = 0;
  mem->addrFirstFree = 0; // Freelist is empty
  mem->addrEnd = numItems * sizeof(VItem); // Address of last item

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

  if (0 < mem->addrFirstFree)
  {
    // ALLOCATE FROM FREELIST

    //PrintLine("Alloc from freelist");

    item = ListMemGet(mem, mem->addrFirstFree);
    mem->addrFirstFree = ItemGetNext(item);
  }
  else
  {
    // ALLOCATE FROM UNUSED MEMORY

    //PrintLine("Alloc from memory");

    if (mem->addrNextFree <= mem->addrEnd)
    {
      item = ListMemGet(mem, mem->addrNextFree);
      mem->addrNextFree += sizeof(VItem);
      // ++ mem->addrNextFree;
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
    // Free allocated buffer
    if (ItemGetPtr(item)) SysFree(ItemGetPtr(item));
  }

  // Set type None and add deallocated item to freelist
  ItemSetType(item, TypeNone);
  ItemSetNext(item, mem->addrFirstFree);
  mem->addrFirstFree = ListMemGetAddr(mem, item);

  printf("dealloc mem->addrFirstFree: %lu\n", (unsigned long) mem->addrFirstFree);
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
  while (IsNotNil(item))
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
  VAddr itemAddr = 1;

  while (itemAddr < mem->addrNextFree)
  {
    VItem* item = ListMemGet(mem, itemAddr);

    if (ItemGetGCMark(item))
    {
      //PrintLine("MemSweep unmark");
      ItemGCMarkUnset(item);
    }
    else
    {
      //PrintLine("MemSweep dealloc");
      ListMemDeallocItem(mem, item);
    }

    //++ itemAddr;
    itemAddr += sizeof(VItem);
  }
}
