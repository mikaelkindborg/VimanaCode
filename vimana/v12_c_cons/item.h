/*
File: item.h
Author: Mikael Kindborg (mikael@kindborg.com)

Items are like cons cells in Lisp. They hold a value and 
an address to the next item.

Items are equal size and allocated from a larger block by the
memory manager in cellmemory.h. 

Items are copied when pushed onto the data stack and 
assigned to variables. The data stack and global variables
have separate memory areas (arrays), which are not handled 
by the memory manager.

Items allocated by the memory manager are garbage collected. 
Items in the data stack and in variables are used as the roots
for garbage collection. 

Importantly, items on the stack and in variables are non-garbage 
collected items. These items are not garbage collected themselves, 
since they are not allocated by the memory manager. However, they 
may point to items in the garbage collected space; for example 
lists.

An item can point to a memory buffer (allocated with malloc).
This is the case with strings, for example. The item pointing
to the string buffer must exist in garbage-collected memory
to be collected, and theye may be only one item that refers to
a malloc allocated memoery buffer.

For this reason, an additional item are used to refer to 
allocated menory buffers.

This is what it looks like for strings:

Data Stack     Managed Mem     Malloc Mem
----------     -----------     ----------
StringItem --> BufferItem  --> MemoryBuffer

For example:

Data Stack     Managed Mem     Malloc Mem
----------     -----------     ----------
StringItem --> BufferItem  --> 'Hi World'

Here is an axample for lists:

Data Stack     Managed Mem
----------     ------------------------
ListHead   --> FirstItem --> SecondItem

Example of a list that contains a list:

Data Stack     Managed Mem
----------     ------------------------
ListHead   --> ListHead   --> SecondItem
                 !
               FirstItem  --> SecondItem

See cellmemory.h for details on how managed memory is allocated 
and how addresses are used to reference items.

Addresses are item indexes, and are used to save space by 
not having to store full pointers to items on small machines 
(Arduino). 

Pointers to allocated memory are stored as full pointers. 

Pointers to primitive functions are stored as full pointers 
in optimized mode.
*/

// -------------------------------------------------------------
// Item struct
// -------------------------------------------------------------

typedef struct __VItem
{
  // Value of the item
  union
  {
    VAddr       first;      // Address of first item in a child list
    VIntNum     intNum;     // Integer value (symbol, integers)
    VDecNum     decNum;     // Floating point number
    VPrimFunPtr primFunPtr; // Pointer to a primitive function
    void*       ptr;        // Pointer to memory block
  };
    
  // Layout of field "next":
  // Bit 1-4: Type info  
  // Bit 5:   GC mark bit
  // Bit 6-n: Address of next item

  // Address of next item in a list - also holds GC mark bit and type info
  VAddr         next;       
}
VItem;

// -------------------------------------------------------------
// Item types
// -------------------------------------------------------------

enum ItemType
{
  TypeNone = 0,
  TypeIntNum,
  TypeDecNum,
  TypeList,
  TypeBufferPtr,   // Is never pushed to the data stack
  TypeBuffer,      // Used for strings (and other memory objects)
  //TypeString,
  //TypeSocket,
  TypeSymbol,      // Pushable types must go before TypeSymbol
  TypePrimFun,     // Primitive function
  TypeFun,         // Vimana function
  //TypeFunX,        // Vimana "macro" function
  __TypeSentinel__
};

#define IsTypeNone(item)         (TypeNone == ItemType(item))
#define IsTypeList(item)         (TypeList == ItemType(item))
#define IsTypeIntNum(item)       (TypeIntNum == ItemType(item))
#define IsTypeDecNum(item)       (TypeDecNum == ItemType(item))
#define IsTypeBufferPtr(item)    (TypeBufferPtr == ItemType(item))
#define IsTypeBuffer(item)       (TypeBuffer == ItemType(item))
//#define IsTypeString(item)       (TypeString == ItemType(item))
//#define IsTypeSocket(item)       (TypeSocket == ItemType(item))
#define IsTypeSymbol(item)       (TypeSymbol == ItemType(item))
#define IsTypePrimFun(item)      (TypePrimFun == ItemType(item))
#define IsTypeFun(item)          (TypeFun == ItemType(item))

// Pushable items are types that are pushed
// to the data stack without being evaluated
#define IsTypePushable(item) \
  (ItemType(item) < TypeSymbol)

// Types that do not reference other items
#define IsTypeAtomic(item) \
  (IsTypeNone(item)  || IsTypeIntNum(item)  || IsTypeDecNum(item) || \
  IsTypeSymbol(item) || IsTypePrimFun(item) || IsTypeBufferPtr(item))

// Empty list
#define IsEmpty(item) \
  (IsTypeList(item) && (0 == (item)->first))

// List types
#define IsList(item) \
  (IsTypeList(item) || IsTypeFun(item))

// -------------------------------------------------------------
// Convert addr to/from pointer
// -------------------------------------------------------------

// Use bit shift to multiply to get item offset 
// 4 byte item size (on 8 bit processors with 16 bit pointers)
//   #define ItemAddrPtrShift 2 
// 8 byte item size (on 32 bit processors)
//   #define ItemAddrPtrShift 3 
// 16 byte item size (on 64 bit processors)
//   #define ItemAddrPtrShift 4
//
// This:
//   ptr = start + (addr * sizeof(VItem))
// is equal to:
//   ptr = start + (addr << ItemAddrPtrShift)

// Use on 64 bit machines
#define ItemAddrPtrShift 4 

// Convert address to pointer
// start is the beginning of cell memory
#define ItemAddrToPtr(addr, start) \
  ( (VItem*) (BytePtrOffset(start, ((addr) - 1) << ItemAddrPtrShift)) ) 

// Convert pointer to address
// start is the beginning of cell memory
#define ItemPtrToAddr(itemPtr, start) \
  ( ((BytePtr(itemPtr) - (start)) >> ItemAddrPtrShift) + 1 )

// TODO: Check that addressing works (+/- 1)
// TODO: Move this to interp and set start to start - sizeof(VItem) in struct
// We cannot have address zero in items since that is empty list or list end.

// -------------------------------------------------------------
// Access to data in item next field
// -------------------------------------------------------------

#define TypeMask         15 // Type bits
#define MarkMask         16 // Mark bit
#define NonAddrMask      31 // TypeMask + MarkMask (non-addr bits)
#define MarkShift        4
#define AddrShift        5

#define ItemGetType(item)   (((item)->next) & TypeMask)
#define ItemGetGCMark(item) ((((item)->next) & MarkMask) >> MarkShift)
#define ItemGetNext(item)   (((item)->next) >> AddrShift)

void ItemSetGCMark(VItem* item, unsigned int mark)
{
  item->next = (item->next & ~MarkMask) | (mark << MarkShift);
}

void ItemSetType(VItem* item, unsigned int type)
{
  item->next = (item->next & ~TypeMask) | type;
}

void ItemSetNext(VItem* item, VAddr addr)
{
  item->next = (item->next & NonAddrMask) | (addr << AddrShift);
}

// -------------------------------------------------------------
// Access to data in item value field
// -------------------------------------------------------------

#define ItemGetFirst(item)  ((item)->addr)
#define ItemGetSymbol(item) ((item)->symbol)
#define ItemGetIntNum(item) ((item)->intNum)
#define ItemGetDecNum(item) ((item)->decNum)

// Set first of child list
void ItemSetFirst(VItem* item, VAddr addr)
{
  item->addr = addr;
}

void ItemSetSymbol(VItem* item, VIntNum symbol)
{
  item->intNum = symbol;
  ItemSetType(item, TypeSymbol);
}

void ItemSetIntNum(VItem* item, VIntNum number)
{
  item->intNum = number;
  ItemSetType(item, TypeIntNum);
}

void ItemSetDecNum(VItem* item, VDecNum number)
{
  item->decNum = number;
  ItemSetType(item, TypeDecNum);
}

#ifdef OPTIMIZE

  #define ItemGetPrimFun(item) ((item)->primFunPtr)

  void ItemSetPrimFun(VItem* item, VPrimFunPtr primFun)
  {
    item->primFunPtr = primFun;
    ItemSetType(item, TypePrimFun);
  }
#else

  #define ItemGetPrimFun(item) ((item)->intNum)

  void ItemSetPrimFun(VItem* item, VIntNum primFun)
  {
    item->intNum = primFun;
    ItemSetType(item, TypePrimFun);
  }

#endif

// -------------------------------------------------------------
// Init item
// -------------------------------------------------------------

void ItemInit(VItem* item)
{
  item->first = 0;
  item->next = 0;
}

// -------------------------------------------------------------
// Item functions
// -------------------------------------------------------------

// TODO: Need to fix this for doubles?
#define ItemEquals(item1, item2) \
  ( (ItemType(item1) == ItemType(item2)) && \
    ((item1)->intNum == (item2)->intNum) )
