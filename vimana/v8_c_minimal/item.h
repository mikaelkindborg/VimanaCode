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
    VmPointer  obj;
    VmNumber   number;
    VmUNumber  bits;
  }
  value;
}
VmItem;

// VIMANA TYPES ------------------------------------------------

// Three low bits are used for type information

#define TypeNumber        1
#define TypeSymbol        2
#define TypePointer       3
#define TypePrimFun       4
#define TypeFun           5
#define TypeVirgin        15

// ITEM TYPE CHECKING ------------------------------------------

#define IsVirgin(item)    ((item).value.bits == 0)
#define IsPointer(item)   (((item).value.bits & 7) == 0)
#define IsNumber(item)    (((item).value.bits & TypeNumber) == TypeNumber)
#define IsSymbol(item)    (((item).value.bits & 2) == 2)
#define IsList(item)      (((item).value.bits & 3) == 3)
#define IsPrimFun(item)   (((item).value.bits & 4) == 4)

// CREATE ITEMS ------------------------------------------------

/*
VmItem ItemWithList(VmList* list)
{
  VmItem item;
  item.value.obj = list;
  return item;
}
*/

VmItem ItemWithNumber(VmNumber number)
{
  if (((number << 3) >> 3) != number) ErrorExit("Number is too large");
  VmItem item;
  item.value.number = (number << 3) | TypeNumber;
  return item;
}

VmNumber ItemNumber(VmItem item)
{
  if (!IsNumber(item)) ErrorExit("Not a number");
  return item.value.number >> 3;
}

// Uninitialized value
VmItem ItemWithVirgin()
{
  VmItem item;
  item.value.bits = 0;
  return item;
}
