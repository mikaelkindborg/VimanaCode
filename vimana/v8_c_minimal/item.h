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
    void*      obj;
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
#define TypeObj           0
#define TypeNumber        1
#define TypeSymbol        2
#define TypePrimFun       3
#define TypeString        4

// OBJECT TYPES

#define TypeList          1
#define TypeFun           2

// ITEM TYPE CHECKING ------------------------------------------

#define IsVirgin(item)    ((item).value.bits == 0)
#define IsObj(item)       (((item).value.bits & TypeBitMask) == TypeObj)
#define IsNumber(item)    (((item).value.bits & TypeBitMask) == TypeNumber)
#define IsSymbol(item)    (((item).value.bits & TypeBitMask) == TypeSymbol)
#define IsPrimFun(item)   (((item).value.bits & TypeBitMask) == TypePrimFun)
#define IsString(item)    (((item).value.bits & TypeBitMask) == TypeString)
#define IsList(item) \
  ( (IsObj(item)) && (TypeList == ((VList*)((item).value.obj))->type) )
#define IsFun(item) \
  ( (IsObj(item)) && (TypeFun == ((VList*)((item).value.obj))->type) )

// CREATE ITEMS ------------------------------------------------

// Create uninitialized value
VItem ItemWithVirgin()
{
  VItem item;
  item.value.bits = 0;
  return item;
}

VItem ItemWithObj(void* obj)
{
  VItem item;
  item.value.obj = obj;
  return item;
}

VItem ItemWithNumber(VNumber number)
{
  if (((number << 3) >> 3) != number) 
    ErrorExit("ItemWithNumber: Number is too large");
  VItem item;
  item.value.number = (number << 3) | TypeNumber;
  return item;
}

void ItemSetNumber(VItem* item, VNumber number)
{
  if (((number << 3) >> 3) != number) 
    ErrorExit("ItemWithNumber: Number is too large");
  item->value.number = (number << 3) | TypeNumber;
}

VItem ItemWithSymbol(VNumber symbolId)
{
  if (((symbolId << 3) >> 3) != symbolId) 
    ErrorExit("ItemWithSymbol: Symbol id is too large");
  VItem item;
  item.value.number = (symbolId << 3) | TypeSymbol;
  return item;
}

VItem ItemWithPrimFun(VNumber primFunId)
{
  if (((primFunId << 3) >> 3) != primFunId) 
    ErrorExit("ItemWithPrimFun: Primfun id is too large");
  VItem item;
  item.value.number = (primFunId << 3) | TypePrimFun;
  return item;
}

// The item takes ownership of the string buffer.
VItem ItemWithString(char* pBuf)
{
  VItem item;
  item.value.bits = (VUNumber)pBuf | TypeString;
  return item;
}

// ACCESS ITEMS ------------------------------------------------

VNumber ItemNumber(VItem item)
{
  if (!IsNumber(item)) 
    ErrorExit("ItemNumber: Not a number");
  return item.value.number >> 3;
}

VNumber ItemSymbol(VItem item)
{
  if (!IsSymbol(item)) 
    ErrorExit("ItemSymbol: Not a symbol");
  return item.value.number >> 3;
}

VNumber ItemPrimFun(VItem item)
{
  if (!IsPrimFun(item)) 
    ErrorExit("ItemPrimFun: Not a primfun");
  return item.value.number >> 3;
}

void* ItemObj(VItem item)
{
  if (!IsObj(item)) 
    ErrorExit("ItemObj: Not a pointer!");
  return item.value.obj;
}

char* ItemString(VItem item)
{
  if (!IsString(item)) 
    ErrorExit("ItemString: Not a string!");
  return (char*)((item.value.bits) & ~TypeString);
}
