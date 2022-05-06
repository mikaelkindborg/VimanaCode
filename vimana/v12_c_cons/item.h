/*
File: item.h
Author: Mikael Kindborg (mikael@kindborg.com)

Items are like conses in Lisp. They hold a value and 
an address to the next item.
*/

typedef struct __VInterp VInterp;
typedef void (*VPrimFunPtr) (VInterp*);

typedef struct __VItem
{
  union
  {
    VAddr       addr;
    VIntNum     intNum;
    VDecNum     decNum;
    VPrimFunPtr primFunPtr;
    void*       ptr;
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
  ( (ItemType(item1) == ItemType(item2)) && \
    ((item1)->intNum == (item2)->intNum) )

enum ItemType
{
  TypeNone = 0,
  TypeList,
  TypeIntNum,
  TypeDecNum,
  TypeString,
  TypeSocket,
  TypeBufferPtr,
  TypeSymbol,      // Pushable types must go before TypeSymbol
  TypePrimFun,
  TypeFun,
  __TypeSentinel__
};

// Pushable items are types that can be pushed to the data stack 
// without being evaluated
#define IsTypePushable(item)     (ItemType(item) < TypeSymbol)
#define IsTypeNone(item)         (TypeNone == ItemType(item))
#define IsTypeList(item)         (TypeList == ItemType(item))
#define IsTypeIntNum(item)       (TypeIntNum == ItemType(item))
#define IsTypeDecNum(item)       (TypeDecNum == ItemType(item))
#define IsTypeString(item)       (TypeString == ItemType(item))
#define IsTypeSocket(item)       (TypeSocket == ItemType(item))
#define IsTypeSymbol(item)       (TypeSymbol == ItemType(item))
#define IsTypePrimFun(item)      (TypePrimFun == ItemType(item))
#define IsTypeFun(item)          (TypeFun == ItemType(item))
#define IsTypeBufferPtr(item)    (TypeBufferPtr == ItemType(item))
// Empty list
#define IsNil(item)              (IsTypeList(item) && ((item)->addr == 0))
// List types
#define IsListType(item)         (IsTypeList(item) || IsTypeFun(item))

void ItemSetGCMark(VItem* item, VType mark)
{
  item->type = (item->type & ~1) | (mark & 1);
}

void ItemSetType(VItem* item, VType type)
{
  item->type = (type << 1) | (item->type & 1);
}

void ItemSetSymbol(VItem* item, VIntNum symbol)
{
  item->intNum = symbol;
  ItemSetType(item, TypeSymbol);
}

#ifdef OPTIMIZE
  void ItemSetPrimFun(VItem* item, VPrimFunPtr primFun)
  {
    item->primFunPtr = primFun;
    ItemSetType(item, TypePrimFun);
  }
#else
  void ItemSetPrimFun(VItem* item, VIntNum primFun)
  {
    item->intNum = primFun;
    ItemSetType(item, TypePrimFun);
  }
#endif

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
