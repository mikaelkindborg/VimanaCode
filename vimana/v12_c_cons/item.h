/*
File: item.h
Author: Mikael Kindborg (mikael@kindborg.com)

Items are like conses in Lisp. They hold a value and 
an address to the next item.

Items are equal size and allocated from a larger block by the
memory manager in itemMemory.h. 

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
ListHead   --> ListHead (FirstItem) --> SecondItem
                 !
               FirstItem  --> SecondItem

See itemMemory.h for details on how managed memory is allocated 
and how addresses are used to reference items.

Addresses are item indexes, and are used to save space by 
not having to store full pointers to items on small machines 
(Arduino). 

Pointers to allocated memory are stored as full pointers. 

Pointers to primitive functions are stored as full pointers 
in optimized mode.
*/

// Forward declarations
typedef struct __VInterp VInterp;
typedef void (*VPrimFunPtr) (VInterp*);

typedef struct __VItem
{
  // Value of the item
  union
  {
    VAddr       addr;       // Address of first item in a child list
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
#define IsNil(item) \
  (IsTypeList(item) && (0 == (item)->addr))

// List types
#define IsList(item) \
  (IsTypeList(item) || IsTypeFun(item))

#define TypeMask         15 // Type bits
#define MarkMask         16 // Mark bit
#define NonAddrMask      31 // TypeMask + MarkMask (non-addr bits)
#define MarkShift        4
#define AddrShift        5

#define ItemType(item)     (((item)->next) & TypeMask)
#define ItemGCMark(item)   ((((item)->next) & MarkMask) >> MarkShift)
#define ItemNextAddr(item) (((item)->next) >> AddrShift)

void ItemSetGCMark(VItem* item, unsigned int mark)
{
  item->next = (item->next & ~MarkMask) | (mark << MarkShift);
}

void ItemSetType(VItem* item, unsigned int type)
{
  item->next = (item->next & ~TypeMask) | type;
}

void ItemSetNextAddr(VItem* item, VAddr addr)
{
  item->next = (item->next & NonAddrMask) | (addr << AddrShift);
}

void ItemSetSymbol(VItem* item, VIntNum symbol)
{
  item->intNum = symbol;
  ItemSetType(item, TypeSymbol);
}

#ifdef OPTIMIZE
  void ItemSetPrimFun(VItem* item, VPrimFunPtr primFun)
  {
    item->primFunPtr = primFun;
    ItemSetType(item, TypePrimFun);
  }
#else
  void ItemSetPrimFun(VItem* item, VIntNum primFun)
  {
    item->intNum = primFun;
    ItemSetType(item, TypePrimFun);
  }
#endif

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

void ItemInit(VItem* item)
{
  item->intNum = 0;
  item->next = 0;
}

// TODO: Need to fix this for doubles?
#define ItemEquals(item1, item2) \
  ( (ItemType(item1) == ItemType(item2)) && \
    ((item1)->intNum == (item2)->intNum) )
