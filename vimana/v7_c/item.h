
/****************** C TYPES ******************/

typedef unsigned long    Type;
typedef int              Index;
typedef long             IntNum;
typedef double           DecNum;
typedef struct MyItem    Item;
typedef struct MyList    List;
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
#define TypeGlobalVar    128  
#define TypeLocalVar     256 
#define TypeString       512
#define TypeVirgin       0  // Represents unbound symbol/uninitialized item

#define IsVirgin(item)      ((item).type == TypeVirgin)
#define IsSymbol(item)      ((item).type & TypeSymbol)
#define IsIntNum(item)      ((item).type & TypeIntNum)
#define IsDecNum(item)      ((item).type & TypeDecNum)
#define IsBool(item)        ((item).type & TypeBool)
#define IsList(item)        ((item).type & TypeList)
#define IsPrimFun(item)     ((item).type & TypePrimFun)
#define IsFun(item)         ((item).type & TypeFun)
#define IsGlobalVar(item)   ((item).type & TypeGlobalVar)
#define IsLocalVar(item)    ((item).type & TypeLocalVar)
#define IsString(item)      ((item).type & TypeString)

/****************** STRUCTS ******************/

// An item encapsulates C data types. Everything in the
// high-level language is an item.
typedef struct MyItem
{
  Type  type;
  union
  {
    // Fields used by data lists and code.
    Index   symbol; // Index in symbol table or local environment table
    DecNum  decNum;
    IntNum  intNum;
    List*   list;
    char*   string; // TODO: Make custom string object
    Bool    truth;
    
    // Field used only by global symbol table items.
    PrimFun primFun;
  }
  value;
}
Item;

/****************** CREATE ITEMS ******************/

/*
#define ItemWithSymbol(item, symbolIndex) \
do { \
  (item).type = TypeSymbol; \
  (item).value.symbol = symbolIndex; \
} while(0)

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
  item.type = TypeList | TypePrimFun;
  item.value.primFun = fun;
  return item;
}

// Unbound/uninitialized value
Item ItemWithVirgin()
{
  Item item;
  item.type = TypeVirgin;
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
#define ItemList(itemWithList) (itemWithList).value.list
#else
List* ItemList(Item itemWithList)
{
  if (!IsList(itemWithList))
    ErrorExit("ItemList: Item is not of TypeList");
  return itemWithList.value.list;
}
#endif

// Get the IntNum of an item.
#ifdef OPTIMIZE 
#define ItemIntNum(item) (item).value.intNum
#else
IntNum ItemIntNum(Item item)
{
  if (!IsIntNum(item))
    ErrorExit("ItemIntNum: Item is not of TypeIntNum");
  return item.value.intNum;
}
#endif

// Get the Bool of an item.
#ifdef OPTIMIZE
#define ItemBool(item) (item).value.truth 
#else
Bool ItemBool(Item item)
{
  if (!IsBool(item))
    ErrorExit("ItemBool: Item is not of TypeBool");
  return item.value.truth;
}
#endif

/****************** EQUALS ******************/

// MOVED THIS CODE TO primfuns.h

/*
Bool ItemEquals(Item a, Item b)
{
  if (IsSymbol(a) && IsSymbol(b))
  {
    return a.value.symbol == b.value.symbol;
  }
  
  if (IsString(a) && IsString(b))
  {
    return StringEquals(a.value.string, b.value.string);
  }
  
  if (IsIntNum(a) && IsIntNum(b))
  {
    return a.value.intNum == b.value.intNum;
  }

  if (IsIntNum(a) && IsDecNum(b))
  {
    return a.value.intNum == b.value.decNum;
  }
  
  if (IsDecNum(a) && IsIntNum(b))
  {
    return a.value.decNum == b.value.intNum;
  }
  
  if (IsDecNum(a) && IsDecNum(b))
  {
    return a.value.decNum == b.value.decNum;
  }
  
  ErrorExit("ItemEquals: Cannot compare items");
}
*/

/****************** ITEM MATH ******************/

// MOVED THIS CODE TO primfuns.h

/*
Item ItemPlus(Item a, Item b)
{
  if (IsIntNum(a) && IsIntNum(b))
    return ItemWithIntNum(a.value.intNum + b.value.intNum);
  
  if (IsIntNum(a) && IsDecNum(b))
    return ItemWithDecNum(a.value.intNum + b.value.decNum);
  
  if (IsDecNum(a) && IsIntNum(b))
    return ItemWithDecNum(a.value.decNum + b.value.intNum);
  
  if (IsDecNum(a) && IsDecNum(b))
    return ItemWithDecNum(a.value.decNum + b.value.decNum);
  
  ErrorExit("ItemPlus: Unsupported item types");
}

// EXPERIMENT
#define ItemMinusP(a, b, res) \
do { \
  if (IsIntNum(*(a)) && IsIntNum(*(b))) \
  { \
    (res)->type = TypeIntNum; \
    (res)->value.intNum = (a)->value.intNum - (b)->value.intNum; \
  } \
  else \
    ErrorExit("ItemMinusP: Unsupported item types"); \
} while(0)

void X_ItemMinusP(Item* a, Item* b, Item* res)
{
  if (IsIntNum(*a) && IsIntNum(*b))
  {
    res->type = TypeIntNum;
    res->value.intNum = a->value.intNum - b->value.intNum;
    return;
  }

  ErrorExit("ItemMinusP: Unsupported item types");
}

Item ItemMinus(Item a, Item b)
{
  if (IsIntNum(a) && IsIntNum(b))
    return ItemWithIntNum(a.value.intNum - b.value.intNum);
  
  if (IsIntNum(a) && IsDecNum(b))
    return ItemWithDecNum(a.value.intNum - b.value.decNum);
  
  if (IsDecNum(a) && IsIntNum(b))
    return ItemWithDecNum(a.value.decNum - b.value.intNum);
  
  if (IsDecNum(a) && IsDecNum(b))
    return ItemWithDecNum(a.value.decNum - b.value.decNum);
  
  ErrorExit("ItemMinus: Unsupported item types");
}

Item ItemTimes(Item a, Item b)
{
  if (IsIntNum(a) && IsIntNum(b))
    return ItemWithIntNum(a.value.intNum * b.value.intNum);

  if (IsIntNum(a) && IsDecNum(b))
    return ItemWithDecNum(a.value.intNum * b.value.decNum);
  
  if (IsDecNum(a) && IsIntNum(b))
    return ItemWithDecNum(a.value.decNum * b.value.intNum);
  
  if (IsDecNum(a) && IsDecNum(b))
    return ItemWithDecNum(a.value.decNum * b.value.decNum);
  
  ErrorExit("ItemTimes: Unsupported item types");
}

Item ItemDiv(Item a, Item b)
{
  if (IsIntNum(a) && IsIntNum(b))
    return ItemWithIntNum(a.value.intNum / b.value.intNum);
  
  if (IsIntNum(a) && IsDecNum(b))
    return ItemWithDecNum(a.value.intNum / b.value.decNum);
  
  if (IsDecNum(a) && IsIntNum(b))
    return ItemWithDecNum(a.value.decNum / b.value.intNum);
  
  if (IsDecNum(a) && IsDecNum(b))
    return ItemWithDecNum(a.value.decNum / b.value.decNum);
  
  ErrorExit("ItemDiv: Unsupported item types");
}

Item ItemModulo(Item a, Item b)
{
  if (IsIntNum(a) && IsIntNum(b))
    return ItemWithIntNum(a.value.intNum % b.value.intNum);
  
  ErrorExit("ItemModulo: Unsupported item types");
}
*/
