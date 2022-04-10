/*
File: item.h
Author: Mikael Kindborg (mikael@kindborg.com)

Items are like conses in Lisp. They hold a value and 
an address to the next item.
*/

typedef unsigned int     VType;
typedef unsigned int     VAddr;
typedef          long    VIntNum;
typedef          double  VDecNum;

typedef struct __VItem
{
  union
  {
    VAddr   addr;
    VIntNum intNum;
    VDecNum decNum;
    // primfun ptr
    char*   string;
  };
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

#define ItemEquals(item1, item2) \
  ( (ItemType(item) == ItemType(item)) && \
    (ItemIntNum(item) == ItemIntNum(item)) )

#define TypeNone          0
#define TypeList          1
#define TypeIntNum        2
#define TypeDecNum        3
#define TypeString        4
#define TypeSymbol        5
#define TypePrimFun       6
#define TypeStringHolder  7

// Items that can be pushed without being evaluated
#define IsPushableItem(item) (ItemType(item) < TypeSymbol)

#define IsTypeNone(item)    (TypeNone == ItemType(item))
#define IsTypeList(item)    (TypeList == ItemType(item))
#define IsTypeIntNum(item)  (TypeIntNum == ItemType(item))
#define IsTypeDecNum(item)  (TypeDecNum == ItemType(item))
#define IsTypeString(item)  (TypeString == ItemType(item))
#define IsTypeSymbol(item)  (TypeSymbol == ItemType(item))
#define IsTypePrimFun(item) (TypePrimFun == ItemType(item))
#define IsTypeStringHolder(item) (TypeStringHolder == ItemType(item))

void ItemSetGCMark(VItem* item, VType mark)
{
  item->type = (item->type & ~1) | (mark & 1);
}

void ItemSetType(VItem* item, VType type)
{
  item->type = (type << 1) | (item->type & 1);
}
/*
void ItemSetAddr(VItem* item, VAddr addr)
{
  item->addr = addr;
}
*/
void ItemSetSymbol(VItem* item, VIntNum symbol)
{
  item->intNum = symbol;
  ItemSetType(item, TypeSymbol);
}

void ItemSetPrimFun(VItem* item, VIntNum primFun)
{
  item->intNum = primFun;
  ItemSetType(item, TypePrimFun);
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

void ItemInit(VItem* item)
{
  item->intNum = 0;
  item->type = 0;
  item->next = 0;
}
