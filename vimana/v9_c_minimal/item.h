/*
File: item.h
Author: Mikael Kindborg (mikael@kindborg.com)

Items are objects that hold values. They are used in lists and on the data stack.
*/

// STRUCTS -----------------------------------------------------

// Data item used in lists
typedef struct __VItem
{
  union
  {
    void*     obj;
    VNumber   number;
    VUNumber  bits;
  }
  value;
}
VItem;

// VIMANA TYPES ------------------------------------------------

// Three low bits are used for type information.
// Object pointers have all zero values in the three low bits.

#define TypeBitMask       7
#define TypeObj           0 // Used for VList and VString
#define TypeNumber        1
//#define TypeSymbol        2
//#define TypePrimFun       4
#define TypeSymbol        3
#define TypePrimFun       7

// OBJECT TYPES

#define TypeList          1
#define TypeFun           2
#define TypeString        3

// ITEM TYPE CHECKING ------------------------------------------

#define IsVirgin(item)    ((item)->value.bits == 0)
//#define IsObj(item)       (((item)->value.bits & TypeBitMask) == TypeObj)
#define IsObj(item)       (((item)->value.bits & 1) == TypeObj)
//#define IsNumber(item)    (((item)->value.bits & 1) == TypeNumber)
#define IsNumber(item)    (((item)->value.bits & TypeBitMask) == TypeNumber)
#define IsBool(item)      IsNumber(item)
#define IsSymbol(item)    (((item)->value.bits & TypeBitMask) == TypeSymbol)
#define IsPrimFun(item)   (((item)->value.bits & TypeBitMask) == TypePrimFun)
#define IsList(item) \
  ( (IsObj(item)) && (TypeList == ((VList*)((item)->value.obj))->header.type) )
#define IsFun(item) \
  ( (IsObj(item)) && (TypeFun == ((VList*)((item)->value.obj))->header.type) )
#define IsString(item) \
  ( (IsObj(item)) && (TypeString == ((VList*)((item)->value.obj))->header.type) )

// SET ITEMS ------------------------------------------------

// Create uninitialized value
void ItemSetVirgin(VItem* item)
{
  item->value.bits = 0;
}

// The item takes ownership of the object.
void ItemSetObj(VItem* item, void* obj)
{
  item->value.obj = obj;
}

void orig_ItemSetNumber(VItem* item, VNumber number)
{
  if (((number << 1) >> 1) != number) 
    GuruMeditation(ITEM_NUMBER_TOO_LARGE);
  item->value.number = (number << 1) | TypeNumber;
}

void ItemSetNumber(VItem* item, VNumber number)
{
  if (((number << 3) >> 3) != number) 
    GuruMeditation(ITEM_NUMBER_TOO_LARGE);
  item->value.number = (number << 3) | TypeNumber;
}

#define ItemSetBool(item, booleanValue) \
  ItemSetNumber(item, (VBool)booleanValue)

void ItemSetSymbol(VItem* item, VNumber symbolId)
{
  if (((symbolId << 3) >> 3) != symbolId) 
    GuruMeditation(ITEM_SYMBOL_TOO_LARGE);
  item->value.number = (symbolId << 3) | TypeSymbol;
}

void ItemSetPrimFun(VItem* item, VNumber primFunId)
{
  if (((primFunId << 3) >> 3) != primFunId) 
    GuruMeditation(ITEM_PRIMFUNID_TOO_LARGE);
  item->value.number = (primFunId << 3) | TypePrimFun;
}

// ACCESS ITEMS ------------------------------------------------

VNumber ItemNumber(VItem* item)
{
  if (!IsNumber(item)) 
    GuruMeditation(ITEM_NOT_NUMBER);
  return item->value.number >> 3;
}

VNumber ItemBool(VItem* item)
{
  if (!IsBool(item)) 
    GuruMeditation(ITEM_NOT_BOOL);
  return item->value.number >> 1;
}

VNumber ItemSymbol(VItem* item)
{
  if (!IsSymbol(item)) 
    GuruMeditation(ITEM_NOT_SYMBOL);
  return item->value.number >> 3;
}

VNumber ItemPrimFun(VItem* item)
{
  if (!IsPrimFun(item)) 
    GuruMeditation(ITEM_NOT_PRIMFUN);
  return item->value.number >> 3;
}

void* ItemObj(VItem* item)
{
  if (!IsObj(item)) 
    GuruMeditation(ITEM_NOT_POINTER);
  return item->value.obj;
}

// COMPARE ITEMS -----------------------------------------------

#define ItemEquals(item1, item2) \
  ( (((VItem*)(item1))->value.bits) == (((VItem*)(item2))->value.bits) )

/* 
//UNUSED
char* ItemString(VItem* item)
{
  if (!IsString(item)) 
    GuruMeditation(ITEM_NOT_STRING);
  return (char*)((item->value.bits) & ~TypeString);
}
*/
