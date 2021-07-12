
// C TYPES -----------------------------------------------------

typedef unsigned int     Type;
typedef long             Index;
typedef long             IntNum;
typedef double           DecNum;
typedef struct MyItem    Item;
typedef struct MyList    List;
typedef struct MyContext Context;
typedef struct MyInterp  Interp;
typedef void   (*PrimFun)(Interp*);

// VIMANA TYPES ------------------------------------------------

#define TypeSymbol        1
#define TypeIntNum        2
#define TypeDecNum        4
#define TypeBool          8
#define TypeList          16
#define TypePrimFun       32
#define TypeFun           64
#define TypeString        128
#define TypeContext       256
#define TypeDynAlloc      512
#define TypeVirgin        0 // Represents unbound symbol/uninitialized item

#define IsVirgin(item)    ((item).type == TypeVirgin)
#define IsSymbol(item)    ((item).type & TypeSymbol)
#define IsIntNum(item)    ((item).type & TypeIntNum)
#define IsDecNum(item)    ((item).type & TypeDecNum)
#define IsBool(item)      ((item).type & TypeBool)
#define IsList(item)      ((item).type & TypeList)
#define IsPrimFun(item)   ((item).type & TypePrimFun)
#define IsFun(item)       ((item).type & TypeFun)
#define IsString(item)    ((item).type & TypeString)
#define IsContext(item)   ((item).type & TypeContext)
#define IsDynAlloc(item)  ((item).type & TypeDynAlloc)

// OP CODES USED BY INTERPRETER

#define OpCodeNone        0
#define OpCodePushItem    1
#define OpCodeEvalSymbol  2
#define OpCodeCallPrimFun 3 

// STRUCTS -----------------------------------------------------

// An item encapsulates C data types. Everything in the
// high-level language is an item.
typedef struct MyItem
{
  Type type;
  Type opCode;
  union
  {
    Index     symbol; // Index in global symbol table
    DecNum    decNum;
    IntNum    intNum;
    List*     list;
    Context*  context;
    char*     string; // TODO: Make custom string object
    Bool      truth;
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
  item.opCode = OpCodeEvalSymbol;
  item.value.symbol = symbolIndex;
  return item;
}

Item ItemWithString(char* string)
{
  Item item;
  item.type = TypeString;
  item.opCode = OpCodePushItem;
  char* stringbuf = MemAlloc(strlen(string) + 1);
  strcpy(stringbuf, string);
  item.value.string = stringbuf;
  return item;
}

Item ItemWithList(List* list)
{
  Item item;
  item.type = TypeList;
  item.opCode = OpCodePushItem;
  item.value.list = list;
  return item;
}

// Uninitialized value
Item ItemWithVirgin()
{
  Item item;
  item.type = TypeVirgin;
  item.opCode = OpCodeNone;
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

// Get the context object of an item.
#ifdef OPTIMIZE
#define ItemContext(item) ((item).value.context)
#else
Context* ItemContext(Item item)
{
  if (!IsContext(item))
    ErrorExit("ItemContext: Item is not of TypeContext");
  else
    return item.value.context;
}
#endif
