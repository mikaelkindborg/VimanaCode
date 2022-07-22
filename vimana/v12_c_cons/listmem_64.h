/*
File: listmem.h
Author: Mikael Kindborg (mikael@kindborg.com)

Memory manager for Lisp-style linked lists.

See item.h for an explanation of memory layout.
*/

// -------------------------------------------------------------
// VListMemory struct
// -------------------------------------------------------------

// Addresses in list memory are pointer offsets (VAddr)

typedef struct __VListMemory
{
  VByte* start;         // Start of memory block
  VAddr  addrNextFree;  // Next free item in memory block
  VAddr  addrFirstFree; // First item in freelist
  VAddr  addrEnd;       // End of memory
  #ifdef TRACK_MEMORY_USAGE
  int    allocCounter;
  #endif
}
VListMemory;

// -------------------------------------------------------------
// Item access
// -------------------------------------------------------------

#define ListMemStart(mem) ((mem)->start)

#ifdef VIMANA_64

  // In 64 bit pointer space the address is a 48 bit pointer

  // The first field of an item is a full 64 bit pointer

  // The next field is a 48 bit full pointer with type info
  // in the high 5 bits

  // The following is for compatibility with 32 and 16 bit code base
  // On 64 bit systems the list memory object is not needed to access
  // the next pointer, but on 32 bit and 16 bit systems it is needed

  #define ListMemGet(mem, addr)      VItemPtr(addr)
  #define ListMemGetAddr(mem, ptr)   ((VAddr) (ptr))
  #define ListMemGetFirst(mem, item) ItemGetFirst(item)
  #define ListMemGetNext(mem, item)  VItemPtr(ItemGetNext(item))

  void ListMemSetFirst(VListMemory* mem, VItem* item, VItem* first)
  {
    ItemSetFirst(item, first);
  }

  void ListMemSetNext(VListMemory* mem, VItem* item, VItem* next)
  {
    ItemSetNext(item, (VAddr) next);
  }

#else

  // In 32 and 16 bit pointer space the address is an index that
  // starts at 1. 
  //
  // On 32 bit systems 27 bits are available for the index,
  // which can address 1GB of memory, or 134M 8 byte items.
  //
  // On 16 bit systems 11 bits are available for the index,
  // which can address 8 KB of memory, or 2K 4 byte items.

  // The index is multiplied (left shifted) by the item size to
  // get the index address offset, and is divided (right shifted) 
  // to get the index address from a pointer offset

  #define AddrToPtr(addr, start)   VItemPtr((start) + ((addr) << IndexShift))
  #define PtrToAddr(ptr, start)    ((VAddr) ((VBytePtr(ptr) - (start)) >> IndexShift))
  #define ListMemGet(mem, addr)    AddrToPtr(addr, ListMemStart(mem)))
  #define ListMemGetAddr(mem, ptr) PtrToAddr(ptr, ListMemStart(mem))

  // The value of the first field is a full pointer

  #define ListMemGetFirst(mem, item) ItemGetFirst(item)

  void ListMemSetFirst(VListMemory* mem, VItem* item, VItem* first)
  {
    ItemSetFirst(item, first);
  }

  // The value of the next field is an index

  #define ListMemGetNext(mem, item) \
    (ItemGetNext(item) ? ListMemGet(mem, ItemGetNext(item)) : NULL)

  void ListMemSetNext(VListMemory* mem, VItem* item, VItem* next)
  {
    ItemSetNext(item, ListMemGetAddr(mem, next));
  }

#endif

// -------------------------------------------------------------
// Initialize
// -------------------------------------------------------------

// Return size in bytes of VListMemory header plus item memory size
int ListMemByteSize(int numItems)
{
  return sizeof(VListMemory) + (numItems * ItemSize());
}

void ListMemInit(VListMemory* mem, int numItems)
{
  //VAddr memByteSize = ListMemByteSize(numItems);

  #ifdef VIMANA_64
    // Addresses are pointers
    mem->start = VBytePtr(mem) + sizeof(VListMemory);
    mem->addrNextFree = (VAddr) mem->start; // Address of next free item
    mem->addrFirstFree = 0;                 // Freelist is empty
    mem->addrEnd = 
      ((VAddr) mem->start) + 
      (numItems * ItemSize());              // End of address space
  #else
    // Addresses are indexes
    mem->start = (VBytePtr(mem) + sizeof(VListMemory)) - ItemSize();
    mem->addrNextFree = 1;                  // Index of next free item
    mem->addrFirstFree = 0;                 // Freelist is empty
    mem->addrEnd = numItems + 1;            // End of address space
  #endif

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

    if (mem->addrNextFree < mem->addrEnd)
    {
      //PrintLine("Alloc from memory");
      item = ListMemGet(mem, mem->addrNextFree);
      #ifdef VIMANA_64
        mem->addrNextFree += ItemSize();
      #else
        ++ mem->addrNextFree;
      #endif
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
  if (IsTypeNone(item)) 
  {
    return;
  }

  #ifdef TRACK_MEMORY_USAGE
    -- mem->allocCounter;
  #endif

  if (IsTypeBuffer(item))
  {
    //PrintLine("FREE BUFFER");
    // Free allocated buffer
    if (ItemGetPtr(item)) 
    {
      SysFree(ItemGetPtr(item)); 
    }
  }

  // Set type None and add deallocated item to freelist
  ItemSetType(item, TypeNone);
  ItemSetNext(item, mem->addrFirstFree);
  mem->addrFirstFree = ListMemGetAddr(mem, item);

  //printf("dealloc mem->addrFirstFree: %lu\n", (unsigned long) mem->addrFirstFree);
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
  if (! (IsTypeString(item) || IsTypeHandle(item)) ) return NULL;

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
  #ifdef VIMANA_64
    VByte* p = mem->start;
    VByte* end = VBytePtr(mem->addrNextFree);
  #else
    VByte* p = mem->start + ItemSize();
    VByte* end = VBytePtr(ListMemGet(mem, mem->addrNextFree));
  #endif

  while (p < end)
  {
    VItem* item = VItemPtr(p);

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

    p += ItemSize();
  }
}
