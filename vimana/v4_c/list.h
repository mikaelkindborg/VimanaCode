
/***************************************************************

Massa svammel här:

The global symbol table is a list with string items 
of TypeString. This table is found in interp.h.

The global symbol table works in tandem with the global
value table. Entries that are bound appear in this list
as value items.

Items on the stack are like items in the program code.
Items in the local environment table are the same.

Some type values for lists are:

TypeSymbol - value.symbol refers to index in the symbol table
TypeIntNum - value.intNum holds the integer value
TypeDecNum - value.decNum holds the decimal number value
TypeList   - value.list points to the list

In a function body, local vars refer to entries in the 
local environment table of the stack frame. This type of
item uses the following field:

TypeLocalSymbol - value.symbol is an index to the stackframe 
environment table. The actual item that holds the type and value 
is in the environment table.

Note that array indexes are used in place of string symbols 
(hash maps) to speed up execution. Symbol lookups (in the
local environment and in the global symbol table) always 
use indexed array access.

***************************************************************/

/****************** C TYPES ******************/

typedef unsigned char    Type;
typedef int              Index;
typedef long             IntNum;
typedef double           DecNum;
typedef struct MyItem    Item;
typedef struct MyList    List;
typedef struct MyInterp  Interp;
typedef void   (*PrimFun)(Interp*);

/****************** VIMANA TYPES ******************/

#define TypeSymbol       1  // Symbol/global variable
#define TypeIntNum       4
#define TypeDecNum       8
#define TypePrimFun      2
#define TypeFun          3
#define TypeList         5
#define TypeString       7
#define TypeVirgin       0  // Represents unbound symbol/uninitialized item
//#define TypeStackFrame  10
#define TypeLocalSymbol 11  // Local variable
#define TypeBool        12

#define IsSymbol(item)      ((item).type == TypeSymbol)
#define IsPrimFun(item)     ((item).type == TypePrimFun)
#define IsFun(item)         ((item).type == TypeFun)
#define IsIntNum(item)      ((item).type == TypeIntNum)
#define IsDecNum(item)      ((item).type == TypeDecNum)
#define IsList(item)        (((item).type == TypeList) || ((item).type == TypeFun))
#define IsString(item)      ((item).type == TypeString)
#define IsVirgin(item)      ((item).type == TypeVirgin)
#define IsLocalSymbol(item) ((item).type == TypeLocalSymbol)
#define IsBool(item)        ((item).type == TypeBool)

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
    char*   string;
    Bool    truth;
    
    // Field used only by global symbol table items.
    PrimFun primFun;
  }
  value;
}
Item;

// List (growable array) that holds items.
// Lists are used frequently throughout the implementation,
// for the code and the data stack, but also in place of
// C records for stackframes and function objects. This is
// a bit experimental, I want the C-implementation to share
// as much as possible with the high-level language. These
// structures could then be accessed from the high-level 
// language. The interpreter object could also be a list.
// TODO: Flag to say that list has been freed? GC flag?
typedef struct MyList
{
  int   length;     // Current number of items
  int   maxLength;  // Max number of items
  Item* items;      // Array of items
}
List;

// Initial list array size and how much to grow on each reallocation.
#define ListGrowIncrement 10

/****************** LISTS ******************/

List* ListCreate()
{
  // Alloc list object.
  size_t size = ListGrowIncrement;
  List* list = malloc(sizeof(List));
  list->length = 0;
  list->maxLength = size;

  // Alloc list array.
  size_t arraySize = size * sizeof(Item);
  Item* itemArray = malloc(arraySize);
  list->items = itemArray;

  // Init list array.
  memset(itemArray, 0, arraySize);

  // Return list object.
  return list;
}

// TODO: whatToFree is how deep to free.
// But proper GC should be used instead.
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
  // TODO: Does not compile, reallocarray not found.
  //Item* newArray = reallocarray(list->items, sizeof(Item), newSize);

  // Make space for mnore items.
  size_t newArraySize = newSize * sizeof(Item);
  Item* newArray = realloc(list->items, newArraySize);
  if (NULL == newArray)
    ErrorExit("ListGrow: Out of memory");
  list->items = newArray;
  list->maxLength = newSize;

  // Set new entries in the array to zero.
  size_t prevArraySize = list->length * sizeof(Item);
  size_t numNewBytes = newArraySize - prevArraySize;
  Byte* p = (Byte*) newArray;
  p = p + prevArraySize;
  memset(p, 0, numNewBytes);

  PrintDebug("REALLOC successful in ListGrow");
}

// Returns the index of the new item.
Index ListPush(List* list, Item item)
{
  // Grow list array if needed.
  if (list->length + 1 > list->maxLength)
    ListGrow(list, list->length + ListGrowIncrement);
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
    ListGrow(list, index + ListGrowIncrement);
  if (index >= list->length)
    list->length = index + 1;
  list->items[index] = item;
}

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
  item.type = TypeLocalSymbol;
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
List* ItemList(Item itemWithList)
{
  if (!IsList(itemWithList))
    ErrorExit("ItemList: Item is not of TypeList");
  return itemWithList.value.list;
}

// Get the IntNum of an item.
IntNum ItemIntNum(Item item)
{
  if (!IsIntNum(item))
    ErrorExit("ItemIntNum: Item is not of TypeIntNum");
  return item.value.intNum;
}

// Get the Bool of an item.
Bool ItemBool(Item item)
{
  if (!IsBool(item))
    ErrorExit("ItemBool: Item is not of TypeBool");
  return item.value.truth;
}

// Experimental. Useful for updating item values "in place" 
// without having to copy and write back the item.
Item* ListGetItemPtr(List* list, int index)
{
  if (index >= list->length)
    ErrorExit("ListGetItemPtr: Index out of bounds: %i\n", index);
  return &(list->items[index]);
}

/****************** EQUALS ******************/

Bool ItemEquals(Item a, Item b)
{
  // TODO: What if symbol is bound?, then compare bound values.
  // Or perhaps this is for the caller to do? Probably so.
  // Like compare unevaluated and evaluated values.
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

// TODO: Delete?
// Associative list
/*Item ListLookup(List* list, Index symbolIndex)
{
  // TODO
  return ItemWithVirgin();
}*/
