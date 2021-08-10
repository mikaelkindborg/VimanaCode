/*
File: item.h
Author: Mikael Kindborg (mikael@kindborg.com)

Items are objects that hold values. They are used in lists and on the data stack. Items are always copied in code that uses them, but objects (lists and string) pointed to are not copied. Copying items seems to be fast (items are small).
*/

// ITEM TYPE CHECKING ------------------------------------------

#define IsVirgin(item)    ((item).type == TypeVirgin)
#define IsSymbol(item)    ((item).type & TypeSymbol)
#define IsIntNum(item)    ((item).type & TypeIntNum)
#define IsDecNum(item)    ((item).type & TypeDecNum)
#define IsBool(item)      ((item).type & TypeBool)
#define IsList(item)      ((item).type & TypeList)
#define IsPrimFun(item)   ((item).type & TypePrimFun)
#define IsFun(item)       ((item).type & TypeFun)
#define IsSpecialFun(item)((item).type & TypeSpecialFun)
#define IsString(item)    ((item).type & TypeString)
#define IsClosure(item)   ((item).type & TypeClosure)
#define IsDynAlloc(item)  ((item).type & TypeDynAlloc)
//#define IsPushable(item)  ((item).type & TypePushable)

/*
TODO: UNUSED - REMOVE

// OP CODES USED BY INTERPRETER

#define OpCodeNone        0
#define OpCodePushItem    1
#define OpCodeEvalSymbol  2
#define OpCodeCallPrimFun 3 
*/

// STRUCTS -----------------------------------------------------

// An item encapsulates C data types. Everything in the
// high-level language is an item.
typedef struct MyItem
{
  Type  type;
  Index symbol;
  union
  {
    DecNum    decNum;
    IntNum    intNum;
    Bool      truth;
    List*     list;
    VString*  string;
    PrimFun   primFun;
  }
  value;
}
Item;

// CREATE ITEMS ------------------------------------------------

Item ItemWithSymbol(Index symbolIndex)
{
  Item item;
  item.type = TypeSymbol;
  //item.opCode = OpCodeEvalSymbol;
  item.symbol = symbolIndex;
  return item;
}

Item ItemWithString(char* str)
{
  Item item;
  item.type = TypeString;
  //item.opCode = OpCodePushItem;
  //char* stringbuf = MemAlloc(strlen(string) + 1);
  //strcpy(stringbuf, string);
  //item.value.string = stringbuf;
  item.value.string = StringCreate(str);
  return item;
}

Item ItemWithList(List* list)
{
  Item item;
  item.type = TypeList;
  //item.opCode = OpCodePushItem;
  item.value.list = list;
  return item;
}

// Uninitialized value
Item ItemWithVirgin()
{
  Item item;
  item.type = TypeVirgin;
  //item.opCode = OpCodeNone;
  item.value.list = 0; // Sets value to zero.
  return item;
}

// ITEM ACCESS -------------------------------------------------

// Get the list of an item.
#ifdef OPTIMIZE
#define ItemList(item) ((item).value.list)
#else
List* ItemList(Item item)
{
  if (!IsList(item))
    ErrorExit("ItemList: Item is not of TypeList");
  else
    return item.value.list;
}
#endif
