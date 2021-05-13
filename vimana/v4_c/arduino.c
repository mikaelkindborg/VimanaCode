#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define OPTIMIZE

typedef unsigned char Byte;
typedef int           Bool;
//typedef FILE          MyStream;

#define FALSE 0
#define TRUE  1

#define ListFreeShallow 1
#define ListFreeDeep    2
#define ListFreeDeeper  3

#define StringEquals(s1, s2) (0 == strcmp((s1), (s2)))

#define Print(str, args...)      printf(str, ## args)
#define PrintLine(str, args...)  printf(str "\n", ## args)
#define PrintToStream(stream, str, args...)  fprintf(stream, str, ## args)
#define PrintDebug(str, args...) printf("[DEBUG] " str "\n", ## args)
#define ErrorExit(str, args...)  do { printf("[ERROR] " str "\n", ## args); exit(0); } while (0)


/****************** C TYPES ******************/

typedef unsigned int     Type;
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
#define TypeLocalVar 11  // Local variable
#define TypeBool        12

#define IsSymbol(item)      ((item).type == TypeSymbol)
#define IsPrimFun(item)     ((item).type == TypePrimFun)
#define IsFun(item)         ((item).type == TypeFun)
#define IsIntNum(item)      ((item).type == TypeIntNum)
#define IsDecNum(item)      ((item).type == TypeDecNum)
#define IsList(item)        (((item).type == TypeList) || ((item).type == TypeFun))
#define IsString(item)      ((item).type == TypeString)
#define IsVirgin(item)      ((item).type == TypeVirgin)
#define IsLocalSymbol(item) ((item).type == TypeLocalVar)
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

Item ItemMinus(Item a, Item b)
{
  if (IsIntNum(a) && IsIntNum(b))
  {
    a.value.intNum = (a.value.intNum - b.value.intNum);
    return a;
  }
  
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
  {
    a.value.intNum = (a.value.intNum * b.value.intNum);
    return a;
  }

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
  
  ErrorExit("ItemDiv: Unsupported item types");
}

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

TypeLocalVar - value.symbol is an index to the stackframe 
environment table. The actual item that holds the type and value 
is in the environment table.

Note that array indexes are used in place of string symbols 
(hash maps) to speed up execution. Symbol lookups (in the
local environment and in the global symbol table) always 
use indexed array access.

***************************************************************/

int GMallocCounter = 0;
int GReallocCounter = 0;

/****************** LISTS ******************/

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

List* ListCreate()
{
  // Alloc list object.
  size_t size = ListGrowIncrement;

  //GMallocCounter ++;
  //printf("MALLOC CONUTER: %i\n", GMallocCounter);

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

#ifdef OPTIMIZE
#define ListLength(list) ((list)->length)
#else
int ListLength(List* list)
{
  return list->length;
}
#endif

void ListGrow(List* list, size_t newSize)
{
  // TODO: Does not compile, reallocarray not found.
  //Item* newArray = reallocarray(list->items, sizeof(Item), newSize);

  //GReallocCounter ++;
  //printf("REALLOC CONUTER: %i\n", GReallocCounter);
  
  // Make space for more items.
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

  //PrintDebug("REALLOC successful in ListGrow");
}

#ifdef OPTIMIZE
#define ListPush(list, item) \
do { \
  if ((list)->length + 1 > (list)->maxLength) \
    ListGrow(list, (list)->length + ListGrowIncrement); \
  (list)->items[list->length] = (item); \
  (list)->length++; \
} while (0)
#else
void ListPush(List* list, Item item)
{
  // Grow list array if needed.
  if (list->length + 1 > list->maxLength)
    ListGrow(list, list->length + ListGrowIncrement);
  list->items[list->length] = item;
  list->length++;
  //return list->length - 1; // Index of new item.
}
#endif

Item ListPop(List* list)
{
  if (list->length < 1)
    ErrorExit("ListPop: Cannot pop list of length: %i", list->length);
  list->length--;
  return list->items[list->length];
}

#ifdef OPTIMIZE
#define ListGet(list, index) ((list)->items[index])
#else
Item ListGet(List* list, int index)
{
  if (index >= list->length)
    ErrorExit("ListGet: Index out of bounds: %i\n", index);
  return list->items[index];
}
#endif

#ifdef OPTIMIZE
#define ListSet(list, index, item) \
do { \
  if ((index) >= (list)->maxLength) \
    ListGrow((list), (index) + ListGrowIncrement); \
  if ((index) >= (list)->length) \
    (list)->length = (index) + 1; \
  (list)->items[index] = (item); \
} while (0)
#else
void ListSet(List* list, int index, Item item)
{
  // Grow list if needed.
  if (index >= list->maxLength)
    ListGrow(list, index + ListGrowIncrement);
  if (index >= list->length)
    list->length = index + 1;
  list->items[index] = item;
}
#endif

// Experimental. Useful for updating item values "in place" 
// without having to copy and write back the item.
#ifdef OPTIMIZE
#define ListGetItemPtr(list, index) (&((list)->items[index]))
#else
Item* ListGetItemPtr(List* list, int index)
{
  if (index >= list->length)
    ErrorExit("ListGetItemPtr: Index out of bounds: %i\n", index);
  return &(list->items[index]);
}
#endif

// TODO: Delete?
// Associative list
/*
Item ListLookup(List* list, Index symbolIndex)
{
  // TODO
  return ItemWithVirgin();
}
*/


// StackFrame fields.
#define StackFrameCodeList     0
#define StackFrameCodePointer  1
#define StackFrameEnv          2

// Function fields.
#define FunNumArgs             0
#define FunNumVars             1
#define FunBody                2

// Declarations.
Item InterpEvalSymbol(Interp* interp, Item item);
void InterpEvalList(Interp* interp, List* list);
Item InterpCompileFun(Interp* interp, Item funList);
List* InterpGetLocalEnv(Interp* interp);

//octo: that was new

/****************** INTERPRETER ******************/

typedef struct MyInterp
{
  List* symbolTable;      // List of global names
  List* symbolValueTable; // List of global values (primfuns, funs, ...)
  List* stack;            // The data stack
  List* callstack;        // Callstack with stack frames
  int   stackframeIndex;  // Index of current stack frame
  Bool  run;              // Run flag
}
Interp;

Interp* InterpCreate()
{
  Interp* interp = malloc(sizeof(Interp));
  interp->symbolTable = ListCreate();
  interp->symbolValueTable = ListCreate();
  interp->stack = ListCreate();
  interp->callstack = ListCreate();
  interp->stackframeIndex = -1;
  interp->run = TRUE;
  return interp;
}

void InterpFree(Interp* interp)
{
  ListFree(interp->symbolTable, ListFreeDeep);
  ListFree(interp->symbolValueTable, ListFreeShallow);
  ListFree(interp->stack, ListFreeShallow);
  ListFree(interp->callstack, ListFreeDeep);
}

// Push item on the data stack.
#ifdef OPTIMIZE
#define InterpPush(interp, item) ListPush((interp)->stack, item)
#else
void InterpPush(Interp* interp, Item item)
{
  ListPush(interp->stack, item);
}
#endif

// Pop item off the data stack.
#ifdef OPTIMIZE
#define InterpPop(interp) ListPop((interp)->stack)
#else
Item InterpPop(Interp* interp)
{
  return ListPop(interp->stack);
}
#endif

// Pop item off the data stack and evaluate it
// if it is a symbol.
#ifdef OPTIMIZE
#define InterpPopEval(interp) InterpEvalSymbol(interp, ListPop((interp)->stack))
#else
Item InterpPopEval(Interp* interp)
{
  Item item = ListPop(interp->stack);
  return InterpEvalSymbol(interp, item);
}
#endif

void InterpAddPrimFun(char* name, PrimFun fun, Interp* interp)
{
  // Add name to symbol table.
  ListPush(interp->symbolTable, ItemWithString(name));
  
  // Add function to symbol value table.
  ListPush(interp->symbolValueTable, ItemWithPrimFun(fun));
}

void InterpSetGlobalSymbolValue(Interp* interp, Index index, Item value)
{
  ListSet(interp->symbolValueTable, index, value);
}

void InterpSetLocalSymbolValue(Interp* interp, Index index, Item value)
{
  List* env = InterpGetLocalEnv(interp);
  if (env)
    ListSet(env, index, value);
  else
    ErrorExit("InterpSetLocalSymbolValue: Local environment not found");
}

char* InterpGetSymbolString(Interp* interp, Index symbolIndex)
{
  Item item = ListGet(interp->symbolTable, symbolIndex);
  return item.value.string;
}

/****************** STACKFRAME ******************/

// Stackframe rules: 
// Every stackframe has an environment list.
// The environment is empty for non-function calls.
// Search for local vars is made in the first non-empty
// environment of the current frame or a parent frame.

/*
void StackFrameFree(List* stackframe)
{
  List* env = ItemList(ListGet(stackframe, StackFrameEnv));
  ListFree(env, ListFreeShallow);
  ListFree(stackframe, ListFreeShallow);
}
*/

// New function that returns the next stackframe.
List* InterpObtainStackFrame(Interp* interp)
{
  // 1. Check for tail call optimization.
  if (interp->stackframeIndex > -1)
  {
    List* stackframe = ItemList(ListGet(interp->callstack, interp->stackframeIndex));
    IntNum codePointer = ItemIntNum(ListGet(stackframe, StackFrameCodePointer));
    List* currentCodeList = ItemList(ListGet(stackframe, StackFrameCodeList));

    // Check for tail call.
    if (codePointer + 1 >= ListLength(currentCodeList))
    {
      // Tail call.
      //PrintLine("TAILCALL AT INDEX: %i", interp->stackframeIndex);
      return stackframe;
    }
  }

  // Increment stackframe index.
  interp->stackframeIndex ++;

  // 2. Create new stackframe if needed.
  if (ListLength(interp->callstack) <= interp->stackframeIndex)
  {
    // Create stackframe.
    List* stackframe = ListCreate();
    ListSet(stackframe, StackFrameEnv, ItemWithList(ListCreate()));
    ListSet(interp->callstack, interp->stackframeIndex, ItemWithList(stackframe));
    //PrintLine("NEW STACKFRAME AT INDEX: %i", interp->stackframeIndex);
    return stackframe;
  }

  // 3. Reuse existing stackframe.
  List* stackframe = ItemList(ListGet(interp->callstack, interp->stackframeIndex));
  //PrintLine("ENTER STACKFRAME AT INDEX: %i", interp->stackframeIndex);
  //ListPrint(stackframe, interp);
  return stackframe;
}

void InterpExitStackFrame(Interp* interp)
{
  //PrintDebug("EXIT STACKFRAME: %i", interp->stackframeIndex);
  //List* stackframe = ItemList(ListPop(interp->callstack));
  interp->stackframeIndex--;
  //StackFrameFree(stackframe);
}

List* InterpGetLocalEnv(Interp* interp)
{
  List* stackframe;
  Index stackFrameIndex = interp->stackframeIndex;
  while (stackFrameIndex > -1)
  {
    stackframe = ItemList(ListGet(interp->callstack, stackFrameIndex));

    //PrintDebug("STACKFRAME in InterpGetLocalEnv:");
    //ListPrint(stackframe, interp);

    // Do we have an environment?
    if (ListLength(stackframe) >= StackFrameEnv + 1)
    {
      Item item = ListGet(stackframe, StackFrameEnv);
      if (IsList(item))
      {
        // Return env if it is not empty.
        List* env = ItemList(item);
        if (ListLength(env) > 0)
          return env;
      }
    }
    stackFrameIndex--;
  }
  return NULL;
}

/****************** SYMBOL LOOKUP ******************/

// Lookup the symbol string in the symbol table.
Index InterpLookupSymbolIndex(Interp* interp, char* symbol)
{
  List* symbolTable = interp->symbolTable;
  for (int i = 0; i < ListLength(symbolTable); i++)
  {
    Item item = ListGet(symbolTable, i);
    char* string = item.value.string;
    if (StringEquals(string, symbol))
    {
      // Found it.
      return i;
    }
  }
  return -1; // Not found.
}

// Add a symbol to the symbol table and return an
// item with the entry. Used by the parser.
Item InterpAddSymbol(Interp* interp, char* symbol)
{
  // Lookup the symbol.
  Index index = InterpLookupSymbolIndex(interp, symbol);
  if (index > -1)
  {
    // Symbol is already added, return an item with 
    // the symbol index.
    //PrintDebug("SYMBOL EXISTS IN SYMTABLE: %s\n", symbol);
    Item item = ItemWithSymbol(index);
    return item;
  }
  else
  {
    // Symbol does not exist, create it.
    Item newItem = ItemWithString(symbol);
    ListPush(interp->symbolTable, newItem);
    ListPush(interp->symbolValueTable, ItemWithVirgin());
    Index newIndex = ListLength(interp->symbolTable) - 1;
    // Make a symbol item and return it.
    Item item = ItemWithSymbol(newIndex);
    return item;
  }
}

/****************** EVAL SYMBOL ******************/

// Lookup the value of a symbol (variable value).
// Return Virgin item if no value exists.
Item InterpEvalSymbol(Interp* interp, Item item)
{
  // If the item is a symbol, evaluate it.
  // Evaluating a symbol means finding its value,
  // if it is bound. An unbound symbol evaluates
  // to itself (its literal value).
  
  // Lookup symbol in stackframe local environment.
  if (IsLocalSymbol(item))
  {
    // Get current local environment.
    List* env = InterpGetLocalEnv(interp);
    if (env)
    {
      Item value = ListGet(env, item.value.symbol);
      if (TypeVirgin != value.type) 
        return value;
    }
    else
      ErrorExit("InterpEvalSymbol: Local environment not found");
  }

  // Lookup symbol in global symbol table.
  if (IsSymbol(item))
  {
    Item value = ListGet(interp->symbolValueTable, item.value.symbol);
    if (TypeVirgin != value.type) 
      return value;
  }

  // Otherwise return the item itself (evaluate to itself).
  return item;
}

/****************** EVAL FUNCTION ******************/

// Bind stack parameters and push a stackframe with
// the function body.
void InterpEvalFun(Interp* interp, List* fun)
{
  // Get info for the stackframe of the function.
  IntNum numArgs = ItemIntNum(ListGet(fun, FunNumArgs));
  IntNum numVars = ItemIntNum(ListGet(fun, FunNumVars));
  List* funBody = ItemList(ListGet(fun, FunBody));

  // Get stackframe. 
  List* stackframe = InterpObtainStackFrame(interp);

  // Get environment.
  List* env = ItemList(ListGet(stackframe, StackFrameEnv));
  // Set it to empty.
  env->length = 0;

  //ListSet(env, 0, ItemWithIntNum(42));
  //ListSet(env, 0, ItemWithList(fun));

  // Bind parameters.
  for (int i = numArgs - 1; i >= 0; i--)
  {
    Item arg = InterpPopEval(interp);
    ListSet(env, i, arg);
  }

  // Set all localvars to TypeVirgin.
  for (int i = numArgs; i < numArgs + numVars; i++)
  {
    ListSet(env, i, ItemWithVirgin());
  }

  // Set code and code pointer.
  ListSet(stackframe, StackFrameCodeList, ItemWithList(funBody));
  ListSet(stackframe, StackFrameCodePointer, ItemWithIntNum(-1));

  //PrintDebug("STACKFRAME IN InterpEvalFun:");
  //ListPrint(stackframe, interp);
}

/****************** EVAL LIST ******************/

void InterpEvalList(Interp* interp, List* list)
{
  // Get stackframe. 
  List* stackframe = InterpObtainStackFrame(interp);

  // Set code and code pointer.
  ListSet(stackframe, StackFrameCodeList, ItemWithList(list));
  ListSet(stackframe, StackFrameCodePointer, ItemWithIntNum(-1));

  //PrintDebug("STACKFRAME IN InterpEvalList:");
  //ListPrint(stackframe, interp);
}

/****************** EVAL ELEMENT ******************/

void InterpEval(Interp* interp, Item element)
{
  // Optimize for primfun lookup.
  if (IsSymbol(element))
  {
    Item value = ListGet(interp->symbolValueTable, element.value.symbol);

    // If primitive function, evaluate it.
    if (IsPrimFun(value))
    {
      //PrintDebug("PRIM FOUND: %i", element.value.symbol);
      // Call the primitive.
      value.value.primFun(interp);
      return;
    }
    
    if (IsFun(value))
    {
      //PrintDebug("FUN FOUND: %i", element.value.symbol);
      // Call the function.
      InterpEvalFun(interp, value.value.list);
      return;
    }
  }

  if (IsLocalSymbol(element))
  {
    // Evaluate symbol to see if it is bound to a function.
    Item value = InterpEvalSymbol(interp, element);
    
    if (IsFun(value))
    {
      //PrintDebug("FUN FOUND: %i", element.value.symbol);
      // Call the function.
      InterpEvalFun(interp, ItemList(value));
      return;
    }
  }

  // Otherwise push element onto the data stack (not evaluated).
  ListPush(interp->stack, element);
  //PrintDebug("PUSH ELEMENT ONTO DATA STACK TYPE: %u", element.type);
}

/****************** MAIN INTERPRETER LOOP ******************/

void InterpRun(Interp* interp, List* list)
{
  interp->stackframeIndex = -1;
  interp->run = TRUE;
  
  // Set root stackframe.
  InterpEvalList(interp, list);
  //PrintDebug("CREATED ROOT FRAME AT INDEX: %i", interp->stackframeIndex);
  
  while (interp->run)
  {
    // Get current stackframe.
    List* stackframe = ItemList(ListGet(interp->callstack, interp->stackframeIndex));

    // Increment code pointer.
    /*
    Item codePointer = ListGet(stackframe, StackFrameCodePointer);
    codePointer.value.intNum ++;
    ListSet(stackframe, StackFrameCodePointer, codePointer);
    */
    Item* codePtr = ListGetItemPtr(stackframe, StackFrameCodePointer);
    codePtr->value.intNum ++;

    // If the code in the stackframe has finished executing
    // we exit the frame.
    List* codeList = ItemList(ListGet(stackframe, StackFrameCodeList));

    if (codePtr->value.intNum >= ListLength(codeList))
    {
      // EXIT STACK FRAME
      //InterpExitStackFrame(interp);
      interp->stackframeIndex--;
    }
    else
    {
      // Evaluate the current element. Note that new stackframes
      // may be created during evaluation, and that this will 
      // increment the stackframe index.
      Item element = ListGet(codeList, codePtr->value.intNum);

      // Innlined below: InterpEval(interp, element);

      // Inlining code from InterpEval
      if (IsSymbol(element))
      {
        Item value = ListGet(interp->symbolValueTable, element.value.symbol);
        if (IsPrimFun(value))
        {
          value.value.primFun(interp);
          goto exit;
        }
        if (IsFun(value))
        {
          InterpEvalFun(interp, value.value.list);
          goto exit;
        }
      }
      if (IsLocalSymbol(element))
      {
        Item value = InterpEvalSymbol(interp, element);
        if (IsFun(value))
        {
          InterpEvalFun(interp, ItemList(value));
          goto exit;
        }
      }
      // Otherwise
      ListPush(interp->stack, element);
    }
exit:
    // Was this the last stackframe?
    if (interp->stackframeIndex < 0)
    {
      //PrintDebug("EXIT InterpRun");
      interp->run = FALSE;
    }
  }
}

/***************************************************************

"COMPILING" FUNCTIONS

"Compiling" a listy imnto a function simply means replacing
local symbols with indexes into the local enviropnment table.
The use of indexes should make the lookup faster than using
a string lookup table such as a hash map.

Before compile:

((X) (A B) 
  (1 A SET 
   N A + B SET 
   B))
   
StackFrame env becomes 0:X-value 1:A-value 2:B-value

After compile:

(1 (X A B)
  (1 (VAR 1) SET 
   (VAR 0) (VAR 1) + (VAR 2) SET 
   (VAR 2))) 

Example in lower case:

((x) (a b) 
  (1 a set
   x a + b set
   b))
   
Variation in style:

((x) (a b) 
  ((1 a set) do
   (x a + b set) do
   b))

Example of defining and calling a function:

((X) () (X X +)) FUN DOUBLE SET
21 DOUBLE PRINTLN

***************************************************************/

/****************** "COMPILE" ******************/

int InterpCompileFunLookupLocalIndex(Interp* interp, List* localVars, Item symbol)
{
  for (int i = 0; i < ListLength(localVars); i++)
  {
    Item localSymbol = ListGet(localVars, i);
    if (ItemEquals(symbol, localSymbol)) 
      return i;
  }
  
  // Not a local symbol.
  return -1;
}
  
Item InterpCompileFunReplaceSymbols(Interp* interp, List* localVars, List* bodyList)
{
  List* newList = ListCreate();
  
  for (int i = 0; i < ListLength(bodyList); i++)
  {
    Item item = ListGet(bodyList, i);
    if (IsList(item))
    {
      item = InterpCompileFunReplaceSymbols(interp, localVars, item.value.list);
      ListPush(newList, item);
    }
    else if (IsSymbol(item))
    {
      // Replace symbol if in localvars.
      int index = InterpCompileFunLookupLocalIndex(interp, localVars, item);
      if (index > -1)
        ListPush(newList, ItemWithLocalSymbol(index));
      else
        ListPush(newList, item);
    }
    else
    {
      ListPush(newList, item);
    }
  }
  
  return ItemWithList(newList);
}

// "Compile" the function list by replacing local var symbols 
// with indexes. This should be faster than hashtable lookups.
// Return a list with the compiled function, an item of type: TypeFun
Item InterpCompileFun(Interp* interp, Item funList)
{
  //PrintDebug("Compile Fun");
  
  if (!IsList(funList))
    ErrorExit("InterpCompileFun: funList is not a list");

  int length = ListLength(funList.value.list);

  if (3 != length)
    ErrorExit("InterpCompileFun: Wrong number of elements in funList");

  Item argList  = ListGet(ItemList(funList), 0);
  Item varList  = ListGet(ItemList(funList), 1);
  Item bodyList = ListGet(ItemList(funList), 2);
  
  // Do some basic checks.
  if (!IsList(argList))
    ErrorExit("InterpCompileFun: argList is not a list");
  if (!IsList(varList))
    ErrorExit("InterpCompileFun: varList is not a list");
  if (!IsList(bodyList))
    ErrorExit("InterpCompileFun: bodyList is not a list");
  
  // The resulting list that holds the compiled
  // function has the format: (NUMARGS LOCALVARS BODY)
  // TODO Make this a structs instead?
  
  // Create list for LOCALVARS
  List* localVars = ListCreate();
  int numArgs = ListLength(ItemList(argList));
  int numVars = ListLength(ItemList(varList));

  for (int i = 0; i < numArgs; i++)
    ListPush(localVars, ListGet(ItemList(argList), i));
  for (int i = 0; i < numVars; i++)
    ListPush(localVars, ListGet(ItemList(varList), i));

  // Recursively traverse bodyList and replace local symbols with indexes.
  Item funBody = InterpCompileFunReplaceSymbols(interp, localVars, ItemList(bodyList));
  
  // Create list for the compile function. TODO Struct?
  List* compiledFun = ListCreate();
  
  ListPush(compiledFun, ItemWithIntNum(numArgs));
  ListPush(compiledFun, ItemWithIntNum(numVars));
  ListPush(compiledFun, funBody);

  //PrintDebug("COMPILED FUNCTION:");
  //ListPrint(compiledFun, interp);
  
  // Return item with the compiled list.
  return ItemWithFun(compiledFun);
}


/****************** PARSER ******************/

int InterpParserWorker(Interp* interp, char* code, int i, int length, List* list);

List* InterpParseCode(Interp* interp, char* code)
{
  List* list = ListCreate();
  InterpParserWorker(interp, code, 0, strlen(code), list);
  return list;
}

int ParserIsNumber(char* token)
{
  Bool decimalSignUsed = FALSE;
  for (int i = 0; i < strlen(token); i++)
  {
    char c = token[i];
    if ('.' == c)
    {
      if (decimalSignUsed) return 0;
      decimalSignUsed = TRUE;
    }
    else if (!isdigit(c))
    {
      return 0;
    }
  }
  return decimalSignUsed ? TypeDecNum : TypeIntNum;
}

void InterpParserAddSymbolOrNumber(Interp* interp, char* token, List* list)
{
  Item item;
  int type = ParserIsNumber(token);
  if (TypeIntNum == type)
  {
    long num = strtol(token, NULL, 10);
    //PrintDebug("TOKEN INTNUM: %s %li\n", token, num);
    item = ItemWithIntNum(num);
  }
  else if (TypeDecNum == type)
  {
    double num = strtod(token, NULL);
    //PrintDebug("TOKEN DECNUM: %s %f\n", token, num);
    item = ItemWithDecNum(num);
  }
  else
  {
    item = InterpAddSymbol(interp, token);
  }
  ListPush(list, item);
}

int InterpParserWorker(Interp* interp, char* code, int i, int length, List* list)
{
  int foo = 0;
  char token[512]; // Check buffer overrun
  char* ptoken = NULL;

  while (i < length)
  {
    // Begin list.
    if (code[i] == '(')
    {
      //PrintDebug("BEGIN LIST");
      List* childList = ListCreate();
      Item item = ItemWithList(childList);
      ListPush(list, item);
      i = InterpParserWorker(interp, code, i + 1, length, childList);
      continue;
    }

    // End list.
    if (code[i] == ')')
    {
      //PrintDebug("END LIST");  
      // End token
      if (ptoken)
      {
        *ptoken = 0; // Zero terminate token
        //PrintDebug("ADD TOKEN: %s", token);
        InterpParserAddSymbolOrNumber(interp, token, list);
      }
      return i + 1;
    }
    
    // Copy tokens separated by whitespace.
    // Whitespace separates tokens.
    if (code[i] == ' '  || code[i] == '\t' ||  
        code[i] == '\n' || code[i] == '\r')
    {
      // End token
      if (ptoken)
      {
        
        *ptoken = 0; // Zero terminate token
        ptoken = NULL; // Indicates no token 
        //PrintDebug("ADD TOKEN: %s", token);
        InterpParserAddSymbolOrNumber(interp, token, list);
      }
      i++;
      continue;
    }

    // When we are here, we have a token character.

    // Begin token.
    if (!ptoken)
    {
      ptoken = token;
    }
    
    // Copy char to token
    *ptoken = code[i];
    ptoken++;
    i++;
  }
  
  // End token
  if (ptoken)
  {
    *ptoken = 0; // Zero terminate token
    //PrintDebug("ADD TOKEN: %s", token);
    InterpParserAddSymbolOrNumber(interp, token, list);
  }

  return i;
}


/****************** PRIMFUNS ******************/

void Prim_DROP(Interp* interp)
{
  InterpPop(interp);
}

// DO evaluates a list. Other types generates an error.
void Prim_DO(Interp* interp)
{
  //PrintDebug("HELLO DO");
  Item item = InterpPopEval(interp);
  //PrintDebug("ITEM TYPE: %u", item.type);
  // If item is a list, create a stackframe and push it onto the stack.
  if (IsList(item))
    InterpEvalList(interp, ItemList(item));
  else
    ErrorExit("DO got a non-list of type: %u", item.type);
}

void Prim_IFTRUE(Interp* interp)
{
  Item item = InterpPopEval(interp);
  Bool truth = ItemBool(InterpPopEval(interp));
  if (!IsList(item))
    ErrorExit("IFTRUE got a non-list of type: %u", item.type);
  if (truth)
    InterpEvalList(interp, ItemList(item));
}

void Prim_IFELSE(Interp* interp)
{
  Item branch2 = InterpPopEval(interp);
  Item branch1 = InterpPopEval(interp);
  Bool truth = ItemBool(InterpPopEval(interp));
  if (!(IsList(branch1) && IsList(branch2)))
    ErrorExit("IFELSE got a non-list items");
  if (truth)
    InterpEvalList(interp, ItemList(branch1));
  else
    InterpEvalList(interp, ItemList(branch2));
}

// FUN turns a list into a function.
// Example:
// ((X) () (X X +) FUN DOUBLE SET
void Prim_FUN(Interp* interp)
{
  //PrintDebug("HELLO FUN");
  Item list = InterpPopEval(interp);
  Item compiledFun = InterpCompileFun(interp, list);
  InterpPush(interp, compiledFun);
}

// SET a global symbol to a value.
// Example:
// 42 FOO SET FOO PRINTLN
void Prim_SET(Interp* interp)
{
  //PrintDebug("HELLO SET");
  
  // Get name and value.
  Item name = InterpPop(interp);
  Item value = InterpPopEval(interp);

  //PrintDebug("NAME TYPE:  %u", name.type);
  //PrintDebug("VALUE TYPE: %u", value.type);

  // Check type.
  if (IsLocalSymbol(name))
  {
    //PrintDebug("LOCAL SET");
    InterpSetLocalSymbolValue(interp, name.value.symbol, value);
  }
  else if (IsSymbol(name))
  {
    //PrintDebug("GLOBAL SET");
    InterpSetGlobalSymbolValue(interp, name.value.symbol, value);
  }
  else
  {
    ErrorExit("SET  got a non-symbol of type: %u", name.type);
  }
}

void Prim_PRINTLN(Interp* interp)
{
  Serial.write("HELLO PRINTLN");
}

void Prim_PLUS(Interp* interp)
{
  Item a = InterpPopEval(interp);
  Item b = InterpPopEval(interp);
  Item res = ItemPlus(b, a);
  InterpPush(interp, res);
}

void Prim_MINUS(Interp* interp)
{
  Item a = InterpPopEval(interp);
  Item b = InterpPopEval(interp);
  Item res = ItemMinus(b, a);
  InterpPush(interp, res);
}

void Prim_TIMES(Interp* interp)
{
  Item a = InterpPopEval(interp);
  Item b = InterpPopEval(interp);
  Item res = ItemTimes(b, a);
  InterpPush(interp, res);
}

void Prim_DIV(Interp* interp)
{
  Item a = InterpPopEval(interp);
  Item b = InterpPopEval(interp);
  Item res = ItemDiv(b, a);
  InterpPush(interp, res);
}

void Prim_MODULO(Interp* interp)
{
  Item a = InterpPopEval(interp);
  Item b = InterpPopEval(interp);
  Item res = ItemModulo(b, a);
  InterpPush(interp, res);
}

void Prim_TRUE(Interp* interp)
{
  InterpPush(interp, ItemWithBool(TRUE));
}

void Prim_FALSE(Interp* interp)
{
  InterpPush(interp, ItemWithBool(FALSE));
}

void Prim_NOT(Interp* interp)
{
  Item item = InterpPopEval(interp);
  Bool x = ItemBool(item);
  InterpPush(interp, ItemWithBool(!x));
}

void Prim_EQ(Interp* interp)
{
  Item a = InterpPopEval(interp);
  Item b = InterpPopEval(interp);
  Bool res = ItemEquals(a, b);
  InterpPush(interp, ItemWithBool(res));
}

void Prim_DELAY(Interp* interp)
{
  Item t = InterpPopEval(interp);
  delay(t.value.intNum);
}

void Prim_LED_ON(Interp* interp)
{
  digitalWrite(LED_BUILTIN, HIGH);
}

void Prim_LED_OFF(Interp* interp)
{
  digitalWrite(LED_BUILTIN, LOW);
}

void InterpDefinePrimFuns(Interp* interp)
{
  InterpAddPrimFun("PRINTLN", &Prim_PRINTLN, interp);
  InterpAddPrimFun("LED_ON", &Prim_LED_ON, interp);
  InterpAddPrimFun("LED_OFF", &Prim_LED_OFF, interp);
  InterpAddPrimFun("DELAY", &Prim_DELAY, interp);
  /*
  InterpAddPrimFun("DROP", &Prim_DROP, interp);
  InterpAddPrimFun("drop", &Prim_DROP, interp);
  InterpAddPrimFun("DOC", &Prim_DROP, interp);
  InterpAddPrimFun("doc", &Prim_DROP, interp);
  InterpAddPrimFun("DO", &Prim_DO, interp);
  InterpAddPrimFun("do", &Prim_DO, interp);
  InterpAddPrimFun("IFTRUE", &Prim_IFTRUE, interp);
  InterpAddPrimFun("iftrue", &Prim_IFTRUE, interp);
  InterpAddPrimFun("IFELSE", &Prim_IFELSE, interp);
  InterpAddPrimFun("ifelse", &Prim_IFELSE, interp);
  InterpAddPrimFun("FUN", &Prim_FUN, interp);
  InterpAddPrimFun("fun", &Prim_FUN, interp);
  InterpAddPrimFun("SET", &Prim_SET, interp);
  InterpAddPrimFun("set", &Prim_SET, interp);
  InterpAddPrimFun("PRINTLN", &Prim_PRINTLN, interp);
  InterpAddPrimFun("PRINT", &Prim_PRINTLN, interp);
  InterpAddPrimFun("print", &Prim_PRINTLN, interp);
  InterpAddPrimFun("+", &Prim_PLUS, interp);
  InterpAddPrimFun("-", &Prim_MINUS, interp);
  InterpAddPrimFun("*", &Prim_TIMES, interp);
  InterpAddPrimFun("/", &Prim_DIV, interp);
  InterpAddPrimFun("%", &Prim_MODULO, interp);
  InterpAddPrimFun("TRUE", &Prim_TRUE, interp);
  InterpAddPrimFun("FALSE", &Prim_FALSE, interp);
  InterpAddPrimFun("NOT", &Prim_NOT, interp);
  InterpAddPrimFun("not", &Prim_NOT, interp);
  InterpAddPrimFun("EQ", &Prim_EQ, interp);
  InterpAddPrimFun("eq", &Prim_EQ, interp);
  */
}

List* list;
Interp* interp;

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);

  //delay(3000);
  interp = InterpCreate();
  InterpDefinePrimFuns(interp);
  //list = InterpParseCode(interp, "HELLO_WORLD PRINTLN");
  //InterpRun(interp, list);
  list = InterpParseCode(interp, "LED_ON 3000 DELAY LED_OFF 500 DELAY");
  int len = ListLength(list);
  //InterpFree(interp);
  Serial.print("Hello World");
  Serial.print(len);
}

// the loop function runs over and over again forever
void loop() {
  InterpRun(interp, list);
  /*
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(500);                       // wait for a second
  */
}
