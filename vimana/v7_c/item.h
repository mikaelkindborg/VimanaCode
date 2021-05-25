
// C TYPES -----------------------------------------------------

typedef unsigned long    Type;
typedef int              Index;
typedef long             IntNum;
typedef double           DecNum;
typedef struct MyItem    Item;
typedef struct MyList    List;
typedef struct MyContext Context;
typedef struct MyInterp  Interp;
typedef void   (*PrimFun)(Interp*);

// VIMANA TYPES ------------------------------------------------

#define TypeSymbol       1
#define TypeIntNum       2
#define TypeDecNum       4
#define TypeBool         8
#define TypeList         16
#define TypePrimFun      32
#define TypeFun          64
//#define TypeCompiledFun  128  
#define TypeLocalVar     256 
#define TypeString       512
#define TypeContext      1024
#define TypeDynAlloc     2048
#define TypeVirgin       0 // Represents unbound symbol/uninitialized item

#define IsVirgin(item)      ((item).type == TypeVirgin)
#define IsSymbol(item)      ((item).type & TypeSymbol)
#define IsIntNum(item)      ((item).type & TypeIntNum)
#define IsDecNum(item)      ((item).type & TypeDecNum)
#define IsBool(item)        ((item).type & TypeBool)
#define IsList(item)        ((item).type & TypeList)
#define IsPrimFun(item)     ((item).type & TypePrimFun)
#define IsFun(item)         ((item).type & TypeFun)
//#define IsCompiledFun(item) ((item).type & TypeCompiledFun)
#define IsLocalVar(item)    ((item).type & TypeLocalVar)
#define IsString(item)      ((item).type & TypeString)
#define IsContext(item)     ((item).type & TypeContext)
#define IsDynAlloc(item)    ((item).type & TypeDynAlloc)

// STRUCTS -----------------------------------------------------

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

// CREATE ITEMS ------------------------------------------------

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

// Uninitialized value
Item ItemWithVirgin()
{
  Item item;
  item.type = TypeVirgin;
  item.value.list = 0; // Sets value to zero.
  return item;
}

Item ItemWithBool(Bool boolVal)
{
  Item item;
  item.type = TypeBool;
  item.value.truth = boolVal;
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
