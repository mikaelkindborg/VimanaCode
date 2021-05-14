
/****************** C TYPES ******************/

typedef unsigned long    Type;
typedef int              Index;
typedef long             IntNum;
typedef double           DecNum;
typedef struct MyItem    Item;
typedef struct MyList    List;
typedef struct MyContext Context;
typedef struct MyInterp  Interp;
typedef void   (*PrimFun)(Interp*);

/****************** VIMANA TYPES ******************/

#define TypeSymbol       1
#define TypeIntNum       2
#define TypeDecNum       4
#define TypeBool         8
#define TypeList         16
#define TypePrimFun      32
#define TypeFun          64
//#define TypeGlobalVar    128  
#define TypeLocalVar     256 
#define TypeString       512
#define TypeContext      1024
#define TypeVirgin       0  // Represents unbound symbol/uninitialized item

#define IsVirgin(item)      ((item).type == TypeVirgin)
#define IsSymbol(item)      ((item).type & TypeSymbol)
#define IsIntNum(item)      ((item).type & TypeIntNum)
#define IsDecNum(item)      ((item).type & TypeDecNum)
#define IsBool(item)        ((item).type & TypeBool)
#define IsList(item)        ((item).type & TypeList)
#define IsPrimFun(item)     ((item).type & TypePrimFun)
#define IsFun(item)         ((item).type & TypeFun)
//#define IsGlobalVar(item)   ((item).type & TypeGlobalVar)
#define IsLocalVar(item)    ((item).type & TypeLocalVar)
#define IsString(item)      ((item).type & TypeString)
#define IsContext(item)     ((item).type & TypeContext)

/****************** STRUCTS ******************/

// An item encapsulates C data types. Everything in the
// high-level language is an item.
typedef struct MyItem
{
  Type  type;
  union
  {
    // Fields used by data lists and code.
    Index     symbol; // Index in symbol table or local environment table
    DecNum    decNum;
    IntNum    intNum;
    List*     list;
    Context*  context;
    char*     string; // TODO: Make custom string object
    Bool      truth;
    
    // Field used only by global symbol table items.
    PrimFun primFun;
  }
  value;
}
Item;

/****************** CREATE ITEMS ******************/

/*
EXPERIMENTS

#define ItemWithSymbol(item, symbolIndex) \
do { \
  (item).type = TypeSymbol; \
  (item).value.symbol = symbolIndex; \
} while(0)

#define ItemWithSymbol(item, symbolIndex) \
((item).type = TypeSymbol, (item).value.symbol = symbolIndex)

void ItemWithSymbol(Item* item, Index symbolIndex)
{
  item->type = TypeSymbol;
  item->value.symbol = symbolIndex;
}
*/

Item ItemWithSymbol(Index symbolIndex)
{
  Item item;
  item.type = TypeSymbol;
  item.value.symbol = symbolIndex;
  return item;
}

Item ItemWithString(char* string)
{
  Item item;
  item.type = TypeString;
  char* stringbuf = malloc(strlen(string) + 1);
  strcpy(stringbuf, string);
  item.value.string = stringbuf;
  //PrintDebug("[TypeString: %s]", item.value.string);
  return item;
}

Item ItemWithIntNum(IntNum number)
{
  Item item;
  item.type = TypeIntNum;
  item.value.intNum = number;
  return item;
}

Item ItemWithDecNum(DecNum number)
{
  Item item;
  item.type = TypeDecNum;
  item.value.decNum = number;
  return item;
}

Item ItemWithList(List* list)
{
  Item item;
  item.type = TypeList;
  item.value.list = list;
  return item;
}

Item ItemWithContext(Context* context)
{
  Item item;
  item.type = TypeContext;
  item.value.context = context;
  return item;
}

Item ItemWithFun(List* fun)
{
  Item item;
  item.type = TypeList | TypeFun;
  item.value.list = fun;
  return item;
}

Item ItemWithPrimFun(PrimFun fun)
{
  Item item;
  item.type = TypePrimFun;
  item.value.primFun = fun;
  return item;
}

// Unbound/uninitialized value
Item ItemWithVirgin()
{
  Item item;
  item.type = TypeVirgin;
  item.value.list = NULL; // Sets value to zero.
  return item;
}

Item ItemWithLocalSymbol(Index symbolIndex)
{
  Item item;
  item.type = TypeLocalVar;
  item.value.symbol = symbolIndex;
  return item;
}

Item ItemWithBool(Bool truth)
{
  Item item;
  item.type = TypeBool;
  item.value.truth = truth;
  return item;
}

/****************** ITEM ACCESS ******************/

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

// Get the IntNum of an item.
#ifdef OPTIMIZE 
#define ItemIntNum(item) ((item).value.intNum)
#else
IntNum ItemIntNum(Item item)
{
  if (!IsIntNum(item))
    ErrorExit("ItemIntNum: Item is not of TypeIntNum");
  else
    return item.value.intNum;
}
#endif

// Get the Bool of an item.
#ifdef OPTIMIZE
#define ItemBool(item) ((item).value.truth )
#else
Bool ItemBool(Item item)
{
  if (!IsBool(item))
    ErrorExit("ItemBool: Item is not of TypeBool");
  else
    return item.value.truth;
}
#endif
