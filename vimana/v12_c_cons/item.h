/*
File: item.h
Author: Mikael Kindborg (mikael@kindborg.com)

Items are like cons cells in Lisp. They hold a value and an address
to the next item.
*/

typedef unsigned long    VData;
typedef unsigned int     VFlag;
typedef unsigned int     VAddr;
typedef          long    VIntNum;
typedef          double  VDecNum;

typedef struct __VItem
{
  VData  data;  // value  (number or pointer)
  VFlag  type;  // type info and gc mark bit
  VAddr  next;  // "address" of next item
}
VItem;

// Layout of field "type" on 64 bit machines:
// bit 1: mark bit
// bit 1-31: type info

#define ItemType(item)   ( ((item)->type) >> 1 )
#define ItemGCMark(item) ( ((item)->type) &  1 )
#define ItemNext(item)   ( (item)->next )
#define ItemData(item)   ( (item)->data )

#define ItemEquals(item1, item2) \
  ( (ItemType(item) == ItemType(item)) && \
    (ItemData(item) == ItemData(item)) )

#define TypeList      1
#define TypeIntNum    2
#define TypeDecNum    3
#define TypeString    4
#define TypeSymbol    5
#define TypePrimFun   6

#define IsPushableValue(item) (ItemType(item) < TypeSymbol)

static inline void ItemSetGCMark(VItem* item, VFlag mark)
{
  item->type = (item->type & ~1) | (mark & 1);
}

void ItemSetType(VItem* item, VFlag type)
{
  item->type = (type << 1) | (item->type & 1);
}

void ItemSetData(VItem* item, VData data)
{
  item->data = data;
}

void ItemSetNext(VItem* item, VAddr addr)
{
  item->next = addr;
}

void ItemSetList(VItem* item, VAddr addr)
{
  ItemSetData(item, addr);
  ItemSetType(item, TypeList);
}

void ItemSetIntNum(VItem* item, VIntNum number)
{
  ItemSetData(item, number);
  ItemSetType(item, TypeIntNum);
}

void ItemSetString(VItem* item, char* string)
{
  ItemSetData(item, (VData)string);
  ItemSetType(item, TypeString);
}

void ItemSetSymbol(VItem* item, VIntNum symbol)
{
  ItemSetData(item, symbol);
  ItemSetType(item, TypeSymbol);
}

void ItemSetPrimFun(VItem* item, VIntNum primFun)
{
  ItemSetData(item, primFun);
  ItemSetType(item, TypePrimFun);
}