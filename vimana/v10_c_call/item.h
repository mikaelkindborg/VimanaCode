/*
File: item.h
Author: Mikael Kindborg (mikael@kindborg.com)

Items are objects that hold values. They are used in lists and on the data stack.
*/

// ITEM STRUCT -------------------------------------------------

// Data item used in lists. Holds function pointer and data value.
// Item size is 128 bits on 64 bit machines.
// The function pointer is used for interpreter dispatch of 
// operations on item types and for primitive functions 
// (this means zero branching in the main interpreter loop).

typedef struct __VItem
{
  VFunPtr      fun;
  union
  {
    VObj*      obj;
    VNumber    number;
    VIndex     symbolId;
  }
  value;
}
VItem;

// ACCESS ITEMS ------------------------------------------------

#define ItemObj(item)    ((item)->value.obj)
#define ItemList(item)   ((VList*)ItemObj(item))
#define ItemString(item) ((VString*)ItemObj(item))
#define ItemNumber(item) ((item)->value.number)
#define ItemBool(item)   ItemNumber(item)
#define ItemSymbol(item) ((item)->value.symbolId)
#define ItemFun(item)    ((item)->fun)

// ITEM TYPES --------------------------------------------------

// The item dispatch function is used to determine the object type.
// These functions are defined in interp.c

VFunPtr GInterpListFun;
VFunPtr GInterpSymbolFun;
VFunPtr GInterpNumberFun;
VFunPtr GInterpStringFun;

// If none of the above, the function is a primfun.

// ITEM TYPE CHECKING ------------------------------------------

#define IsVirgin(item)    (ItemFun(item) == 0)
#define IsNumber(item)    (ItemFun(item) == GInterpNumberFun)
#define IsBool(item)      IsNumber(item)
#define IsSymbol(item)    (ItemFun(item) == GInterpSymbolFun)
#define IsList(item)      (ItemFun(item) == GInterpListFun)
#define IsString(item)    (ItemFun(item) == GInterpStringFun)
#define IsObj(item)       (IsList(item) || IsString(item))
#define IsFun(item)       (IsList(item) && ObjIsFun(ItemObj(item)))
#define IsPrimFun(item) \
  ( !IsVirgin(item) && !IsNumber(item) && !IsSymbol(item) && !IsObj(item) )

// SET ITEMS ------------------------------------------------

static inline void ItemSetVirgin(VItem* item)
{
  ItemFun(item) = 0;
  ItemObj(item) = 0;
}

static inline void ItemSetList(VItem* item, VList* list)
{
  ItemFun(item) = GInterpListFun;
  ItemObj(item) = (VObj*)list;
}

static inline void ItemSetString(VItem* item, VString* string)
{
  ItemFun(item) = GInterpStringFun;
  ItemObj(item) = (VObj*)string;
}

static inline void ItemSetNumber(VItem* item, VNumber number)
{
  ItemFun(item) = GInterpNumberFun;
  ItemNumber(item) = number;
}

#define ItemSetBool(item, boolean) ItemSetNumber(item, boolean)

static inline void ItemSetSymbol(VItem* item, VIndex symbolId)
{
  ItemFun(item) = GInterpSymbolFun;
  ItemSymbol(item) = symbolId;
}

static inline void ItemSetPrimFun(VItem* item, VFunPtr fun, VIndex primFunId)
{
  ItemFun(item) = fun;
  ItemSymbol(item) = primFunId;
}

// COMPARE ITEMS -----------------------------------------------

#define ItemEquals(item1, item2) \
  ( ( ItemFun(item1) == ItemFun(item2) ) && \
    ( ItemNumber(item1) == ItemNumber(item2) ) )
