/*
File: item.h
Author: Mikael Kindborg (mikael@kindborg.com)

Items are like conses in Lisp. They hold a value and 
an address to the next item.

Items are equal size and allocated from a larger block by the
memory manager in mem.h. 

Items are copied when pushed onto the data stack and 
assigned to variables. The data stack and global variables
have separate memory areas, which are not handled by the
memory manager.

Items allocated by the memory manager are garbage collected. 
Items in the data stack and in variables are used as the roots
for garbage collection. 

Importantly, items on the stack and in variables are non-garbage 
collected items. These items are not garbage collected themselves, 
since they are not allocated by the memory manager. However, they 
may point to items in the garbage collected space; for example 
lists.

An item can point to a memory buffer allocated with malloc.
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
ListHead   --> FirstItem --> SecondItem
                 !
               ListHead  --> FirstItem  --> SecondItem

See mem.h for details on how managed memory is allocated and how
addresses are used to reference items.

Addresses are pointer offsets, and are used to save space by 
not having to store full pointers in items. Pointers to
allocated memory are stored as full pointers (since they
exist in a separate memory block). Pointers to primitive
functions are also stored as full pointers in optimized mode.
*/

typedef struct __VInterp VInterp;
typedef void (*VPrimFunPtr) (VInterp*);

typedef struct __VItem
{
  union
  {
    VAddr       addr;       // Address of first item in a list
    VIntNum     intNum;     // Integer value (symbol, integers)
    VDecNum     decNum;     // Floating point number
    VPrimFunPtr primFunPtr; // Pointer to a primitive function
    void*       ptr;        // Pointer to malloc allocated block
  };
  VType         type;       // Type info and GC mark bit
  VAddr         next;       // Address of next item in a list
}
VItem;

// Layout of field "type" on 64 bit machines:
// Bit 1:    mark bit
// Bit 1-31: type info

#define ItemType(item)   ( ((item)->type) >> 1 )
#define ItemGCMark(item) ( ((item)->type) &  1 )
#define ItemNext(item)   ( (item)->next )

#define ItemEquals(item1, item2) \
  ( (ItemType(item1) == ItemType(item2)) && \
    ((item1)->intNum == (item2)->intNum) )

enum ItemType
{
  TypeNone = 0,
  TypeIntNum,
  TypeDecNum,
  TypeList,
  TypeBufferPtr,   // Should not be pushed on the data stack
  TypeString,
  TypeSocket,
  TypeSymbol,      // Pushable types must go before TypeSymbol
  TypePrimFun,
  TypeFun,
  __TypeSentinel__
};

#define IsTypeNone(item)         (TypeNone == ItemType(item))
#define IsTypeList(item)         (TypeList == ItemType(item))
#define IsTypeIntNum(item)       (TypeIntNum == ItemType(item))
#define IsTypeDecNum(item)       (TypeDecNum == ItemType(item))
#define IsTypeBufferPtr(item)    (TypeBufferPtr == ItemType(item))
#define IsTypeString(item)       (TypeString == ItemType(item))
#define IsTypeSocket(item)       (TypeSocket == ItemType(item))
#define IsTypeSymbol(item)       (TypeSymbol == ItemType(item))
#define IsTypePrimFun(item)      (TypePrimFun == ItemType(item))
#define IsTypeFun(item)          (TypeFun == ItemType(item))

// Pushable items are types that can be pushed to the data stack 
// without being evaluated
#define IsTypePushable(item) \
  (ItemType(item) < TypeSymbol)

// Types that does not refernce other items
#define IsTypeAtomic(item) \
  (IsTypeNone(item) || IsTypeIntNum(item) || IsTypeDecNum(item) || \
  IsTypeSymbol(item) || IsTypePrimFun(item) || IsTypeBufferPtr(item))

// Empty list
#define IsNil(item) \
  (IsTypeList(item) && ((item)->addr == 0))

// List types
#define IsList(item) \
  (IsTypeList(item) || IsTypeFun(item))

void ItemSetGCMark(VItem* item, VType mark)
{
  item->type = (item->type & ~1) | (mark & 1);
}

void ItemSetType(VItem* item, VType type)
{
  item->type = (type << 1) | (item->type & 1);
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
  item->type = 0;
  item->next = 0;
}
