/*
File: item.h
Author: Mikael Kindborg (mikael@kindborg.com)

Items are like conses in Lisp. They hold a value and 
an address to the next item.

Items are equal size and allocated from a larger block by the
memory manager in mem.h. 

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
StringItem --> BufferPtrItem   --> MemoryBuffer

For example:

Data Stack     Managed Mem     Malloc Mem
----------     -----------     ----------
StringItem --> BufferPtrItem   --> 'Hi World'

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

Note that a list item always is of TypeList (or TypeFun/FunX).
The list item functions as the head of the list. The elements
of the list have types that correspond to the data they hold.

For example, the rest function will put a list item on the
data stack that points to the first item in the rest of the list.

Example program:

   (1 2 3) REST

Stack sequence:

    (1 2 3) // List pushed onto the stack
            // REST is called (note that the function 
            // itself is NOT pushed onto the stack)
    (2 3)   // Resulting list on the stack

The items on the stack in the above example are items of TypeList.
The items in managed memory are 1, 2 and 3. Items that are on the 
stack are not in managed memory.

An item does not use raw pointers to address first and next.
Instead an index is used which references item memory.
Item memory (mem.h) is essentially just an array, and indexes
are used to access items in this array. Such an index is
call *address* or simply *addr*.

Addresses (indexes) are used to save space by not having to 
store full pointers to items. This enables type info to be
kept in the item next field.

See mem.h for details on how managed memory is allocated and 
how addresses are used to reference items.

Pointers to allocated memory are stored as full pointers in the
value field. 

Pointers to primitive functions are stored as full pointers in
the value field in optimized mode.
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
  VType         type;
  VAddr         next;  
}
VItem;

// Cast to VItem*
#define VItemPtr(ptr) ((VItem*)(ptr))

// -------------------------------------------------------------
// Item types
// -------------------------------------------------------------

enum ItemType
{
  TypeNone = 0,
  TypeIntNum,
  TypeDecNum,
  TypeList,
  TypeString,
  TypeHandle,      // Handle to malloc allocated buffer
  TypeSymbol,      // Pushable types must go before TypeSymbol
  TypePrimFun,     // Primitive function
  TypeFun,         // Vimana function
  TypeFunX,        // Vimana "macro" function
  TypeBuffer,      // Never used on the data stack
  __TypeSentinel__
};

// -------------------------------------------------------------
// Access to data in item next field
// -------------------------------------------------------------

// Layout of field "type":
// Bit 1-7: type info
// Bit 8:   mark bit 

#define ItemGetType(item)   ( ((item)->type) & 127 )
#define ItemGetGCMark(item) ( ((item)->type) & 128 ) // Will be 128 if mark bit is set
#define ItemGetNext(item)   ( (item)->next )

void ItemGCMarkSet(VItem* item)
{
  item->type = item->type | 128;
}

void ItemGCMarkUnset(VItem* item)
{
  item->type = item->type & 127;
}

void ItemSetType(VItem* item, VType type)
{
  item->type = (item->type & 128) | (type & 127);
}

void ItemSetNext(VItem* item, VAddr addr)
{
  item->next = addr;
}

// -------------------------------------------------------------
// Item type access
// -------------------------------------------------------------

#define IsTypeNone(item)         (TypeNone == ItemGetType(item))
#define IsTypeList(item)         (TypeList == ItemGetType(item))
#define IsTypeIntNum(item)       (TypeIntNum == ItemGetType(item))
#define IsTypeDecNum(item)       (TypeDecNum == ItemGetType(item))
#define IsTypeString(item)       (TypeString == ItemGetType(item))
#define IsTypeHandle(item)       (TypeHandle == ItemGetType(item))
#define IsTypeSymbol(item)       (TypeSymbol == ItemGetType(item))
#define IsTypePrimFun(item)      (TypePrimFun == ItemGetType(item))
#define IsTypeFun(item)          (TypeFun == ItemGetType(item))
#define IsTypeFunX(item)         (TypeFunX == ItemGetType(item))
#define IsTypeBuffer(item)       (TypeBuffer == ItemGetType(item))

// Pushable items are types that are pushed
// to the data stack without being evaluated
#define IsTypePushable(item) \
  (ItemGetType(item) < TypeSymbol)

// Types that do not reference other items
#define IsTypeAtomic(item) \
  (IsTypeNone(item)  || IsTypeIntNum(item)  || IsTypeDecNum(item) || \
  IsTypeSymbol(item) || IsTypePrimFun(item) || IsTypeBuffer(item))

// List types
#define IsList(item) \
  (IsTypeList(item) || IsTypeFun(item) || IsTypeFunX(item))

// Empty list
#define IsEmpty(item) \
  (IsList(item) && (0 == ItemGetFirst(item)))

// -------------------------------------------------------------
// Access to data in item value field
// -------------------------------------------------------------

#define ItemGetFirst(item)  ((item)->first)
#define ItemGetSymbol(item) ((item)->intNum)
#define ItemGetIntNum(item) ((item)->intNum)
#define ItemGetDecNum(item) ((item)->decNum)
#define ItemGetPtr(item)    ((item)->ptr)

// Set first of child list
void ItemSetFirst(VItem* item, VAddr addr)
{
  item->first = addr;
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
// Do this in another way?
#define ItemEquals(item1, item2) \
  ( (ItemGetType(item1) == ItemGetType(item2)) && \
    ((item1)->intNum == (item2)->intNum) )
