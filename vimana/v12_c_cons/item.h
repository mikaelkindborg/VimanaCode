/*
File: item.h
Author: Mikael Kindborg (mikael@kindborg.com)

Items are like conses in Lisp. They hold a value and 
an address to the next item.
*/

typedef unsigned long    VData;
typedef unsigned int     VType;
typedef unsigned int     VAddr;
typedef          long    VIntNum;
typedef          double  VDecNum;

typedef struct __VItem
{
  VData  data;  // value  (number or pointer)
  VType  type;  // type info and gc mark bit
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

#define TypeNone      0
#define TypeList      1
#define TypeSymbol    2
#define TypePrimFun   3
#define TypeIntNum    4
#define TypeDecNum    5
#define TypeString    6

#define IsPushableValue(item) (ItemType(item) < TypeSymbol)

static inline void ItemSetGCMark(VItem* item, VType mark)
{
  item->type = (item->type & ~1) | (mark & 1);
}

void ItemSetType(VItem* item, VType type)
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

void ItemInit(VItem* item)
{
  item->data = 0;
  item->type = 0;
  item->next = 0;
}
