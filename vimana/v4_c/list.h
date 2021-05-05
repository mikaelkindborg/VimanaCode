
/****************** C TYPES ******************/

typedef unsigned char Type;
typedef int Index;
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
#define TypeObj          6
#define TypeString       7
#define TypeVirgin       9  // Represents unbound symbol
#define TypeStackFrame  10
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
#define IsStackFrame(item)  ((item.type) == (TypeStackFrame))
#define IsLocalSymbol(item) ((item.type) == (TypeLocalSymbol))

/****************** LISTS ******************/

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
    double  decNum;
    long    intNum;
    List*   list;
    void*   obj;
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

List* ListCreate()
{
  int size = 10;
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

// Returns the index of the new item.
Index ListPush(List* list, Item item)
{
  // Grow list array if needed.
  if (list->length + 1 > list->maxLength)
  {
    size_t newSize = list->length + 10;
    Item* newArray = realloc(list->items, newSize * sizeof(Item));
    // TODO: Does not compile:
    //Item* newArray = reallocarray(list->items, sizeof(Item), newSize);
    
    if (NULL == newArray)
    {
      ErrorExit("Out of memory in ListPush");
    }
    list->items = newArray;
    //PrintDebug("REALLOC successful in ListPush");
  }
  
  list->items[list->length] = item;
  list->length++;
  return list->length - 1; // Index of new item.
}

Item ListPop(List* list)
{
  if (list->length < 1)
  {
    ErrorExit("ERROR: ListPop cannot pop list of length: %i", list->length);
  }
  list->length--;
  return list->items[list->length];
}

Item ListGet(List* list, int index)
{
  if (index >= list->length)
  {
    printf("ERROR: ListGet out of bounds at index: %i\n", list->length);
    exit(0);
  }
  return list->items[index];
}

// TODO: Grow on set
void ListSet(List* list, int index, Item item)
{
  if (index >= list->length)
  {
    printf("ERROR: ListSet out of bounds at index: %i\n", list->length);
    exit(0);
  }
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

Item ItemWithIntNum(long number)
{
  Item item;
  item.type = TypeIntNum;
  item.value.intNum = number;
  return item;
}

Item ItemWithDecNum(double number)
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

Item ItemWithStackFrame(void* obj)
{
  Item item;
  item.type = TypeStackFrame;
  item.value.obj = obj;
  return item;
}

Item ItemWithLocalSymbol(Index symbolIndex)
{
  Item item;
  item.type = TypeLocalSymbol;
  item.value.symbol = symbolIndex;
  return item;
}

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
