/*
File: item.h
Author: Mikael Kindborg (mikael@kindborg.com)

Items are objects that hold values. They are used in lists and on the data stack.
*/

// STRUCTS -----------------------------------------------------

// Data item used in lists
typedef struct __VmItem
{
  union
  {
    void*      obj;
    VmNumber   number;
    VmUNumber  bits;
  }
  value;
}
VmItem;

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
  ( (IsObj(item)) && (TypeList == ((VmList*)((item).value.obj))->type) )
#define IsFun(item) \
  ( (IsObj(item)) && (TypeFun == ((VmList*)((item).value.obj))->type) )

// CREATE ITEMS ------------------------------------------------

// Create uninitialized value
VmItem ItemWithVirgin()
{
  VmItem item;
  item.value.bits = 0;
  return item;
}

VmItem ItemWithObj(void* obj)
{
  VmItem item;
  item.value.obj = obj;
  return item;
}

VmItem ItemWithNumber(VmNumber number)
{
  if (((number << 3) >> 3) != number) 
    ErrorExit("ItemWithNumber: Number is too large");
  VmItem item;
  item.value.number = (number << 3) | TypeNumber;
  return item;
}

void ItemSetNumber(VmItem* item, VmNumber number)
{
  if (((number << 3) >> 3) != number) 
    ErrorExit("ItemWithNumber: Number is too large");
  item->value.number = (number << 3) | TypeNumber;
}

VmItem ItemWithSymbol(VmNumber symbolId)
{
  if (((symbolId << 3) >> 3) != symbolId) 
    ErrorExit("ItemWithSymbol: Symbol id is too large");
  VmItem item;
  item.value.number = (symbolId << 3) | TypeSymbol;
  return item;
}

VmItem ItemWithPrimFun(VmNumber primFunId)
{
  if (((primFunId << 3) >> 3) != primFunId) 
    ErrorExit("ItemWithPrimFun: Primfun id is too large");
  VmItem item;
  item.value.number = (primFunId << 3) | TypePrimFun;
  return item;
}

// The item takes ownership of the string buffer.
VmItem ItemWithString(char* pBuf)
{
  VmItem item;
  item.value.bits = (VmUNumber)pBuf | TypeString;
  return item;
}

// ACCESS ITEMS ------------------------------------------------

VmNumber ItemNumber(VmItem item)
{
  if (!IsNumber(item)) 
    ErrorExit("ItemNumber: Not a number");
  return item.value.number >> 3;
}

VmNumber ItemSymbol(VmItem item)
{
  if (!IsSymbol(item)) 
    ErrorExit("ItemSymbol: Not a symbol");
  return item.value.number >> 3;
}

VmNumber ItemPrimFun(VmItem item)
{
  if (!IsPrimFun(item)) 
    ErrorExit("ItemPrimFun: Not a primfun");
  return item.value.number >> 3;
}

void* ItemObj(VmItem item)
{
  if (!IsObj(item)) 
    ErrorExit("ItemObj: Not a pointer!");
  return item.value.obj;
}

char* ItemString(VmItem item)
{
  if (!IsString(item)) 
    ErrorExit("ItemString: Not a string!");
  return (char*)((item.value.bits) & ~TypeString);
}
