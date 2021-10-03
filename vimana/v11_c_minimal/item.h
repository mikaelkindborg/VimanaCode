/*
File: item.h
Author: Mikael Kindborg (mikael@kindborg.com)

Items are objects that hold values. They are used in lists and on the data stack.
*/

// ITEM STRUCT -------------------------------------------------

// Data item used in lists.

typedef struct __VItem
{
  union
  {
    VObj*     obj;
    VNumber   number;
    VBits     bits;
  }
  value;
}
VItem;

// ITEM TYPES --------------------------------------------------

// Three low bits are used for type information.
// Pointers have    0 in the low bit (type VObj, VList, VString)
// Numbers  have   01 in the low bits
// Symbols  have  011 in the low bits
// PrimFuns have  111 in the low bits

#define TypeBitMask1      1
#define TypeBitMask2      3
#define TypeBitMask3      7
#define TypeObj           0 // Use TypeBitMask1
#define TypeNumber        1 // Use TypeBitMask2
#define TypeSymbol        3 // Use TypeBitMask3
#define TypePrimFun       7 // Use TypeBitMask3

// ITEM TYPE CHECKING ------------------------------------------

#define IsVirgin(item)    ( (item)->value.bits == 0 )
#define IsNumber(item)    ( ((item)->value.bits & TypeBitMask2) == TypeNumber )
#define IsSymbol(item)    ( ((item)->value.bits & TypeBitMask3) == TypeSymbol )
#define IsPtr(item)       ( ((item)->value.bits & TypeBitMask1) == TypeObj )
#define IsObj(item)       ( IsPtr(item) && !IsVirgin(item) )
#define IsPrimFun(item)   ( ((item)->value.bits & TypeBitMask3) == TypePrimFun )
#define IsList(item)      ( (IsObj(item)) && (ObjIsList(ItemObj(item))) )
#define IsString(item)    ( (IsObj(item)) && (ObjIsString(ItemObj(item))) )
#define IsFun(item)       ( (IsObj(item)) && (ObjIsFun(ItemObj(item))) )
#define IsBool(item)      IsNumber(item)

// ACCESS ITEMS ------------------------------------------------

#define ItemObj(item)     ( (item)->value.obj )
#define ItemList(item)    ( (VList*)ItemObj(item) )
#define ItemString(item)  ( (VString*)ItemObj(item) )
#define ItemNumber(item)  ( (item)->value.number >> 2 )
#define ItemSymbol(item)  ( (item)->value.number >> 3 )
#define ItemPrimFun(item) ( (item)->value.number >> 3 )
#define ItemBool(item)    ItemNumber(item)

// SET ITEMS ------------------------------------------------

#define ItemSetList(item, list)     ItemSetObj(item, AsObj(list))
#define ItemSetString(item, string) ItemSetObj(item, AsObj(string))
#define ItemSetBool(item, boolean)  ItemSetNumber(item, boolean)

// Set uninitialized value.
static inline void ItemSetVirgin(VItem* item)
{
  item->value.bits = 0;
}

// The item takes ownership of the object.
static inline void ItemSetObj(VItem* item, VObj* obj)
{
  item->value.obj = obj;
}

static inline void ItemSetNumber(VItem* item, VNumber number)
{
  if (((number << 2) >> 2) != number) 
    GuruMeditation(ITEM_NUMBER_TOO_LARGE);
  item->value.number = (number << 2) | TypeNumber;
}

static inline void ItemSetSymbol(VItem* item, VNumber symbolId)
{
  if (((symbolId << 3) >> 3) != symbolId) 
    GuruMeditation(ITEM_SYMBOL_TOO_LARGE);
  item->value.number = (symbolId << 3) | TypeSymbol;
}

static inline void ItemSetPrimFun(VItem* item, VNumber primFunId)
{
  if (((primFunId << 3) >> 3) != primFunId) 
    GuruMeditation(ITEM_PRIMFUNID_TOO_LARGE);
  item->value.number = (primFunId << 3) | TypePrimFun;
}

// COMPARE ITEMS -----------------------------------------------

#define ItemEquals(item1, item2) \
  ( (((VItem*)(item1))->value.bits) == (((VItem*)(item2))->value.bits) )
