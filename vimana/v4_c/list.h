
/****************** C TYPES ******************/

typedef unsigned char Type;
typedef int Index;
typedef long IntNum;
typedef double DecNum;
typedef struct MyList List;
typedef struct MyItem Item;
typedef struct MyInterp Interp;
typedef void (*PrimFun)(Interp*);

char* InterpGetSymbolString(Interp* interp, Index index);

/****************** VIMANA TYPES ******************/

#define TypeSymbol       1  // Symbol/global variable
#define TypeIntNum       4
#define TypeDecNum       8
#define TypePrimFun      2
#define TypeFun          3
#define TypeList         5
//#define TypeObj          6
#define TypeString       7
#define TypeVirgin       9  // Represents unbound symbol
//#define TypeStackFrame  10
#define TypeLocalSymbol 11  // Local variable

#define IsSymbol(item)      ((item.type) == (TypeSymbol))
#define IsPrimFun(item)     ((item.type) == (TypePrimFun))
#define IsFun(item)         ((item.type) == (TypeFun))
#define IsIntNum(item)      ((item.type) == (TypeIntNum))
#define IsDecNum(item)      ((item.type) == (TypeDecNum))
#define IsList(item)        ((item.type) == (TypeList))
//#define IsObj(item)         ((item.type) == (TypeObj))
#define IsString(item)      ((item.type) == (TypeString))
#define IsVirgin(item)      ((item.type) == (TypeVirgin))
//#define IsStackFrame(item)  ((item.type) == (TypeStackFrame))
#define IsLocalSymbol(item) ((item.type) == (TypeLocalSymbol))

/****************** STRUCTS ******************/

// Flag to say that list has been freed? GC flag?

typedef struct MyList
{
  int   length;     // Current number of items
  int   maxLength;  // Max number of items
  Item* items;      // Array of items
}
List;

typedef struct MyItem
{
  Type  type;
  union
  {
    // Fields used by data lists and code.
    Index   symbol; // Index in symbol table or 
                    // in the local environment table
    DecNum  decNum;
    IntNum  intNum;
    List*   list;
    //void*   obj;
    char*   string;
    
    // Fields only used by items in the symbol table.
    PrimFun primFun;
    
    // Interpreter objects.
    // TODO: StackFrame* stackframe;
    // TODO: Fun*
  }
  value;
}
Item;

/***

Massa svammel här:

The global symbol table is a list with string items 
of TypeString.

The global symbol table works in tandem with the global
value table. Entries that are bound appear in this list
as value items.

Items on the stack are like items in the program code.
Items in the local environment table are the same.

Possible type values for lists are:

TypeSymbol - value.symbol refers to index in the symbol table
TypeIntNum - value.intNum holds the integer value
TypeDecNum - value.decNum holds the decimal number value
TypeList   - value.list points to the list

In a function body, local vars refer to entries in the 
local environment table of the stack frame. This type of
item uses the following field:

TypeLocalSymbol - value.symbol is an index to the stackframe 
environment table

The actual item that holds the type and value is in the 
environment table

In the symbol table the symbol field is always used and the value field is used if the symbol is bound (a global variable).

Note that array indexes are used in place of string symbols to
speed up execution. Environment lookups are always indexes.

***/

/****************** LISTS ******************/

List* ListCreate()
{
  size_t size = 10;
  List* list = malloc(sizeof(List));
  list->length = 0;
  list->maxLength = size;
  Item* itemArray = malloc(size * sizeof(Item));
  list->items = itemArray;
  return list;
}

void ListFree(List* list, int whatToFree)
{
  free(list->items);
  free(list);
}

int ListLength(List* list)
{
  return list->length;
}

void ListGrow(List* list, size_t newSize)
{
  Item* newArray = realloc(list->items, newSize * sizeof(Item));
  // TODO: Does not compile, reallocarray not found.
  //Item* newArray = reallocarray(list->items, sizeof(Item), newSize);
  if (NULL == newArray)
    ErrorExit("ListGrow: Out of memory");
  list->items = newArray;
  list->maxLength = newSize;
  //PrintDebug("REALLOC successful in ListGrow");
}

// Returns the index of the new item.
Index ListPush(List* list, Item item)
{
  // Grow list array if needed.
  if (list->length + 1 > list->maxLength)
    ListGrow(list, list->length + 10);
  list->items[list->length] = item;
  list->length++;
  return list->length - 1; // Index of new item.
}

Item ListPop(List* list)
{
  if (list->length < 1)
    ErrorExit("ListPop: Cannot pop list of length: %i", list->length);
  list->length--;
  return list->items[list->length];
}

Item ListGet(List* list, int index)
{
  if (index >= list->length)
    ErrorExit("ListGet: Index out of bounds: %i\n", index);
  return list->items[index];
}

void ListSet(List* list, int index, Item item)
{
  // Grow list if needed.
  if (index >= list->maxLength)
    ListGrow(list, index + 10);
  if (index >= list->length)
    list->length = index + 1;
  list->items[index] = item;
}

/****************** ITEM LIST ACESS ******************/

/*
Item ItemListCreate()
{
  return ItemWithList(ListCreate());
}

void ItemListFree(Item itemWithList, int whatToFree)
{
  if (!IsList(itemWithList))
    ErrorExit("ItemListFree: Item is not of TypeList");
  ListFree(itemWithList.value.list, whatToFree);
}

void ItemGetList(Item itemWithList)
{
  if (!IsList(itemWithList))
    ErrorExit("ItemGetList: Item is not of TypeList");
  return itemWithList.value.list;
}

int ItemListLength(Item itemWithList)
{
  if (!IsList(itemWithList))
    ErrorExit("ItemListLength: Item is not of TypeList");
  return ListLength(itemWithList.value.list);
}

Index ItemListPush(Item itemWithList, Item item)
{
  if (!IsList(itemWithList))
    ErrorExit("ItemListPush: Item is not of TypeList");
  return ListPush(itemWithList.value.list, item);
}

Item ItemListPop(Item itemWithList)
{
  if (!IsList(itemWithList))
    ErrorExit("ItemListPop: Item is not of TypeList");
  return ListPop(itemWithList.value.list);
}

Item ItemListGet(Item itemWithList, int index)
{
  if (!IsList(itemWithList))
    ErrorExit("ItemListGet: Item is not of TypeList");
  return ListGet(itemWithList.value.list, index);
}

void ItemListSet(Item itemWithList, int index, Item item)
{
  if (!IsList(itemWithList))
    ErrorExit("ItemListSet: Item is not of TypeList");
  ListSet(itemWithList.value.list, index, item);
}

// Experimental
Item* ListGetItemPtr(List* list, int index)
{
  if (index >= list->length)
    ErrorExit("ListGetItemPtr: Index out of bounds: %i\n", index);
  return &(list->items[index]);
}

// Experimental
List* ListGetItemList(List* list, int index)
{
  if (index >= list->length)
    ErrorExit("ListGetItemList: Index out of bounds: %i\n", index);
  Item item = list->items[index];
  if (!IsList(item))
    ErrorExit("ListGetItemList: Item is not of TypeList");
  return item.value.list;
}
*/

/****************** CREATE ITEMS ******************/

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
  item.type = TypeFun;
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

/*Item ItemWithObj(void* obj)
{
  Item item;
  item.type = TypeObj;
  item.value.obj = obj;
  return item;
}*/

// Unbound value type.
Item ItemWithVirgin()
{
  Item item;
  item.type = TypeVirgin;
  return item;
}
/*
Item ItemWithStackFrame(List* stackframe)
{
  Item item;
  item.type = TypeStackFrame;
  item.value.list = stackframe;
  return item;
}
*/
Item ItemWithLocalSymbol(Index symbolIndex)
{
  Item item;
  item.type = TypeLocalSymbol;
  item.value.symbol = symbolIndex;
  return item;
}

/****************** ITEM ACCESS ******************/

List* ItemList(Item itemWithList)
{
  if (!IsList(itemWithList))
    ErrorExit("ItemList: Item is not of TypeList");
  return itemWithList.value.list;
}

// Unused
/*IntNum ItemIntNum(Item item)
{
  if (!IsIntNum(item))
    ErrorExit("ItemIntNum: Item is not of TypeIntNum");
  return item.value.intNum;
}*/

/****************** EQUALS ******************/

Bool ItemEquals(Item a, Item b)
{
  // TODO: What if symbol is bound?, then compare bound values.
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
  exit(0);
}

/****************** ITEM MATH ******************/

Item ItemAdd(Item a, Item b)
{
  if (IsIntNum(a) && IsIntNum(b))
  {
    return ItemWithIntNum(a.value.intNum + b.value.intNum);
  }
  
  if (IsIntNum(a) && IsDecNum(b))
  {
    return ItemWithDecNum(a.value.intNum + b.value.decNum);
  }
  
  if (IsDecNum(a) && IsIntNum(b))
  {
    return ItemWithDecNum(a.value.decNum + b.value.intNum);
  }
  
  if (IsDecNum(a) && IsDecNum(b))
  {
    return ItemWithDecNum(a.value.decNum + b.value.decNum);
  }
  
  ErrorExit("ItemAdd: Cannot add items of this type");
  exit(0);
}
