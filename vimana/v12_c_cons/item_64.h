/*
File: item.h
Author: Mikael Kindborg (mikael@kindborg.com)

# Items

Items are like conses in Lisp. They hold a value and 
an address to the next item.

Items have equal size and allocated from a larger block by the
memory manager in listmem.h. 

## Item memory, lists, and garbage collection

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
may point to items in the garbage collected space, for example 
lists.

Here is an example:

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
The list item is the head of the list. The elements in the
list have types that correspond to the data they hold.

For example, the rest function will put a list item on the
data stack that points to the first item in the rest of the list.

Example program:

   (1 2 3) REST

Stack sequence:

    (1 2 3) // List pushed onto the stack
            // rest is called (note that the function 
            // itself is NOT pushed onto the stack)
    (2 3)   // Resulting list on the stack

Data Stack     Managed Mem
----------     ------------------------
ListHead   --> 1 -> 2 -> 3 (before call to rest)
ListHead   --> 2 -> 3      (after call to rest)

The item on the stack in the above example has type TypeList.
Item 1, 2 and 3 are in managed memory and have type TypeIntNum. 
After rest is called, item 1 can be garbage collected if no 
other list is pointing to it. The list item on the stack are 
not in managed memory.

## Pointers and addresses

Vimana runs on systems with 16 bit, 32 bit or 64 bit pointers.
Note that 8 bit processors like the one on Arduino Uno, use
16 bit pointers.

Depending on the pointer size, pointers or addresses are used
to refer to the next item in a list.

The reference to the first item in a list is always a full 
pointer stored in the value field of a list item.

The next item field has type info and a garbage collection 
mark bit in the high 5 bits. On 64 bit systems it is possible
to use full pointers since a pointer in reality is less that
59 bits (commonly a 64 bit pointer is 48 bits). However, on
16 bit and 32 bit systems, a full pointer does not fit into the
next field, and on these systems an address that is an array
index is used.

To clarify further, items have two fields (see definition of 
struct VItem below). The first field is the value field that 
contains the value of the item. This can be for example a number
of a pointer. In the case of a list item, the value is a
full pointer to the first item in the list. The second field 
holds type info and a pointer or an address to the next item
in the list. If there is no next item, the address is zero,
indicating that the list ends there.

List memory (listmem.h) is essentially just an array, and indexes
can therefore be used to access items in this array on systems
with 16 bit or 32 bit pointers. This scheme enables type info to
be kept in the high bits of the next field.

See listmem.h for details on how managed memory is allocated and 
how addresses are used to reference items.

References to info about using pointers to store extra data
on 64 bit systems:

https://stackoverflow.com/questions/16198700/using-the-extra-16-bits-in-64-bit-pointers

https://craftinginterpreters.com/optimization.html

## Strings and memory buffers

An item can point to a memory buffer (allocated with malloc).
This is the case with strings, for example. The item pointing
to the string buffer must exist in garbage-collected memory
to be collected, and there may be only one item that refers to
a malloc allocated memory buffer.

For this reason, an additional item is used to refer to the
allocated menory buffer.

This is what it looks like for strings:

Data Stack     Managed Mem     Malloc Mem
----------     -----------     ----------
StringItem --> BufferItem      --> MemoryBuffer

For example:

Data Stack     Managed Mem    Malloc Mem
----------     -----------    ----------
StringItem --> BufferItem     --> 'Hi World'

String items are of TypeString, and there is also a TypeHandle
which is used to refer to other types of allocated memory
buffers, like images or data.

## Primitive functions

Pointers to primitive functions are stored as full pointers in
the item value field in optimized mode. This is results in the
best performance, and the function pointers are inserted by
the parser. In non-optimized mode, the id of the function is used.
See code in item.h (this file) and file parser.h for details.

Much more can be said about the interpreter architecture, but
the above gives an introduction to the memory model used, which
is a very important part of the system.
*/

// -------------------------------------------------------------
// Item struct
// -------------------------------------------------------------

typedef struct __VItem
{
  // Value of the item
  union
  {
    VIntNum     intNum;     // Integer value (symbols and integers)
    VDecNum     decNum;     // Floating point value
    VPrimFunPtr primFunPtr; // Pointer to a primitive function
    void*       ptr;        // Pointer to a memory block or 
                            // the first child in a list
  };

  // Type info, gc mark bit, and address to the next item in a list
  VAddr         next;
}
VItem;

//https://stackoverflow.com/questions/6326338/why-when-to-use-intptr-t-for-type-casting-in-c


// Cast to VItem*
#define VItemPtr(ptr) ((VItem*)(ptr))

// This looks better
#define ItemSize() sizeof(VItem)

// -------------------------------------------------------------
// Item types
// -------------------------------------------------------------

#ifdef VIMANA_64

  // In 64 bit pointer space we use 48 bit pointers with type 
  // info and mark bit in the 5 high bits

  #define TypeNone     0x0000000000000000
  #define TypeIntNum   0x1000000000000000
  #define TypeDecNum   0x2000000000000000
  #define TypeList     0x3000000000000000
  #define TypeString   0x4000000000000000
  #define TypeHandle   0x5000000000000000
  #define TypeSymbol   0x6000000000000000 // Pushable types must go before TypeSymbol
  #define TypePrimFun  0x7000000000000000
  #define TypeFun      0x8000000000000000
  #define TypeBuffer   0x9000000000000000

  #define MarkBit      0x0800000000000000
  #define TypeMask     0xF000000000000000
  #define TagMask      0xF800000000000000

#elif VIMANA_32

  // In 32 bit pointer space we use 27 bit address indexes with 
  // type info and mark bit in the 5 high bits

  #define TypeNone     0x00000000
  #define TypeIntNum   0x10000000
  #define TypeDecNum   0x20000000
  #define TypeList     0x30000000
  #define TypeString   0x40000000
  #define TypeHandle   0x50000000
  #define TypeSymbol   0x60000000 // Pushable types must go before TypeSymbol
  #define TypePrimFun  0x70000000
  #define TypeFun      0x80000000
  #define TypeBuffer   0x90000000

  #define MarkBit      0x08000000
  #define TypeMask     0xF0000000
  #define TagMask      0xF8000000

  #define IndexShift   3

#elif VIMANA_16

  // In 16 bit pointer space we use 11 bit address indexes with 
  // type info and mark bit in the 5 high bits

  #define TypeNone     0x0000
  #define TypeIntNum   0x1000
  #define TypeDecNum   0x2000
  #define TypeList     0x3000
  #define TypeString   0x4000
  #define TypeHandle   0x5000
  #define TypeSymbol   0x6000 // Pushable types must go before TypeSymbol
  #define TypePrimFun  0x7000
  #define TypeFun      0x8000
  #define TypeBuffer   0x9000

  #define MarkBit      0x0800
  #define TypeMask     0xF000
  #define TagMask      0xF800

  #define IndexShift   2

#else

  #error Pointer size must be defined (VIMANA_16, VIMANA_32, or VIMANA_64)

#endif

// -------------------------------------------------------------
// Access to data in item next field
// -------------------------------------------------------------

#define ItemGetType(item)   ( ((item)->next) & TypeMask )
#define ItemGetGCMark(item) ( ((item)->next) & MarkBit )
#define ItemGetNext(item)   ( ((item)->next) & ~TagMask )

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
  item->next = next | (item->next & TagMask);
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

// Pushable types are items that are pushed
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
