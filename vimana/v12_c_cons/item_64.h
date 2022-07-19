/*
File: item.h
Author: Mikael Kindborg (mikael@kindborg.com)

Items are like conses in Lisp. They hold a value and 
an address to the next item.

Items are equal size and allocated from a larger block by the
memory manager in itemmemory.h. 

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

Note that a list item always is of TypeList or TypeFun.
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
Item memory (itemmemory.h) is essentially just an array, and indexes
are used to access items in this array. Such an index is
call *address* or simply *addr*.

Addresses (indexes) are used to save space by not having to 
store full pointers to items. This enables type info to be
kept in the item next field.

See itemmemory.h for details on how managed memory is allocated and 
how addresses are used to reference items.

Pointers to allocated memory are stored as full pointers in the
value field. 

Pointers to primitive functions are stored as full pointers in
the value field in optimized mode.

Using pointers as an alternative to addresses:

https://stackoverflow.com/questions/16198700/using-the-extra-16-bits-in-64-bit-pointers
https://craftinginterpreters.com/optimization.html
*/

// -------------------------------------------------------------
// Item struct
// -------------------------------------------------------------

typedef struct __VItem
{
  // Value of the item
  union
  {
    VIntNum     intNum;     // Integer value (symbola and integers)
    VDecNum     decNum;     // Floating point number
    VPrimFunPtr primFunPtr; // Pointer to a primitive function
    void*       ptr;        // Pointer to memory block or first child in list
  };
  VAddr         next;       // Pointer to next in list and type info and gc mark bit
}
VItem;


//#include <stdint.h>

//https://stackoverflow.com/questions/6326338/why-when-to-use-intptr-t-for-type-casting-in-c


// Cast to VItem*
#define VItemPtr(ptr) ((VItem*)(ptr))

// This looks better
#define ItemSize() sizeof(VItem)

// -------------------------------------------------------------
// Item types
// -------------------------------------------------------------

#define TypeNone    0x0000000000000000
#define TypeIntNum  0x1000000000000000
#define TypeDecNum  0x2000000000000000
#define TypeList    0x3000000000000000
#define TypeString  0x4000000000000000
#define TypeHandle  0x5000000000000000
#define TypeSymbol  0x6000000000000000
#define TypePrimFun 0x7000000000000000
#define TypeFun     0x8000000000000000
#define TypeBuffer  0x9000000000000000

#define MarkBit      0x0800000000000000
#define TypeMask     0xF000000000000000
#define TypeMarkMask 0xF800000000000000

// -------------------------------------------------------------
// Access to data in item next field
// -------------------------------------------------------------

#define ItemGetType(item)   ( ((item)->next) & TypeMask )
#define ItemGetGCMark(item) ( ((item)->next) & MarkBit )
#define ItemGetNext(item)   ( ((item)->next) & ~0xF800000000000000 )

void ItemGCMarkSet(VItem* item)
{
  item->next = item->next | MarkBit;
}

void ItemGCMarkUnset(VItem* item)
{
  item->next = item->next & ~MarkBit;
}

void ItemSetType(VItem* item, VType type)
{
  item->next = type | (item->next & ~TypeMask);
}

void ItemSetNext(VItem* item, VAddr next)
{
  item->next = next | (item->next & TypeMarkMask);
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
#define IsList(item) (IsTypeList(item) || IsTypeFun(item))

// Empty list
#define IsEmpty(list) (0 == ItemGetFirst(list))

// -------------------------------------------------------------
// Access to data in item value field
// -------------------------------------------------------------

#define ItemGetSymbol(item) ((item)->intNum)
#define ItemGetIntNum(item) ((item)->intNum)
#define ItemGetDecNum(item) ((item)->decNum)
#define ItemGetFirst(item)  VItemPtr((item)->ptr)
#define ItemGetPtr(item)    ((item)->ptr)

// Set first of child list
void ItemSetFirst(VItem* item, VItem* first)
{
  item->ptr = VItemPtr(first);
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

  void ItemSetPrimFun(VItem* item, VIntNum primFunId)
  {
    VPrimFunPtr primFun = PrimFunTableGet(primFunId)->fun;
    item->primFunPtr = primFun;
    ItemSetType(item, TypePrimFun);
  }

  char* ItemGetPrimFunName(VItem* item)
  {
    int index = PrimFunTableLookupByFunPtr(ItemGetPtr(item));
    return PrimFunTableGet(index)->name;
  }

#else

  #define ItemGetPrimFun(item) (PrimFunTableGet(ItemGetIntNum(item))->fun)

  void ItemSetPrimFun(VItem* item, VIntNum primFunId)
  {
    ItemSetIntNum(item, primFunId);
    ItemSetType(item, TypePrimFun);
  }

  char* ItemGetPrimFunName(VItem* item)
  {
    int index = ItemGetIntNum(item);
    return PrimFunTableGet(index)->name;
  }

#endif

// -------------------------------------------------------------
// Init item
// -------------------------------------------------------------

void ItemInit(VItem* item)
{
  item->ptr   = 0;
  //item->type  = TypeNone;
  item->next  = 0;
}

// -------------------------------------------------------------
// Item functions
// -------------------------------------------------------------

// TODO: Need to fix this for doubles?
// Do this in another way?
#define ItemEquals(item1, item2) \
  ( (ItemGetType(item1) == ItemGetType(item2)) && \
    ((item1)->intNum == (item2)->intNum) )
