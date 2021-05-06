
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

// Invocation object/activation frame
/*
typedef struct MyStackFrame
{
  List* env; // index -> value
  List* fun; // compiled form: (numargs varlist body)
  List* codeList; // funbody or list
  int   codePointer; // position in codelist
}
StackFrame;
*/
// TODO: Why not make StackFrame a list?
// Performance impact? Overhead?
// Or make Fun a struct?

Item InterpCompileFun(Interp* interp, Item funList);

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
  // TODO: Do custom free functions for each type.
  ListFree(interp->symbolTable, ListFreeDeep);
  ListFree(interp->symbolValueTable, ListFreeShallow);
  ListFree(interp->stack, ListFreeShallow);
  ListFree(interp->callstack, ListFreeDeep);
}

// StackFrame fields.
#define StackFrameEnv          0
#define StackFrameCodeList     1
#define StackFrameCodePointer  2

// Experiment using a list to represent a stackframe, 
// rather than a struct.
List* StackFrameCreate(List* codeList)
{
  List* stackframe = ListCreate();
  ListSet(stackframe, StackFrameEnv, ItemWithList(ListCreate()));
  ListSet(stackframe, StackFrameCodeList, ItemWithList(codeList));
  ListSet(stackframe, StackFrameCodePointer, ItemWithIntNum(-1));
  return stackframe;
}

void StackFrameFree(List* stackframe)
{
  List* env = ItemList(ListGet(stackframe, StackFrameEnv));
  ListFree(env, ListFreeShallow);
  ListFree(stackframe, ListFreeShallow);
  // TODO: GC
}

/*
StackFrame* StackFrameCreate(List* codeList)
{
  StackFrame* stackframe = malloc(sizeof(StackFrame));
  stackframe->env = ListCreate();
  stackframe->codeList = codeList;
  stackframe->codePointer = -1;
  return stackframe;
}

void StackFrameFree(StackFrame* stackframe)
{
  ListFree(stackframe->env, ListFreeShallow);
  free(stackframe);
}
*/

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
// item with the entry.
Item InterpAddSymbol(Interp* interp, char* symbol)
{
  // Lookup the symbol.
  Index index = InterpLookupSymbolIndex(interp, symbol);
  if (index > -1)
  {
    // Symbol is already added, return an item with 
    // the symbol index.
    printf("SYMBOL EXISTS IN SYMTABLE: %s\n", symbol);
    Item item = ItemWithSymbol(index);
    return item;
  }
  else
  {
    // Symbol does not exist, create it.
    Item newItem = ItemWithString(symbol);
    Index newIndex = ListPush(interp->symbolTable, newItem);
    ListPush(interp->symbolValueTable, ItemWithVirgin());
    // Make a symbol item and return it.
    Item item = ItemWithSymbol(newIndex);
    return item;
  }
}

char* InterpGetSymbolString(Interp* interp, Index symbolIndex)
{
  Item item = ListGet(interp->symbolTable, symbolIndex);
  return item.value.string;
}

// Lookup the value of a symbol (variable value).
// Return Virgin item if no value exists.
Item InterpEvalSymbol(Interp* interp, Item item)
{
  // If the item is a symbol, evaluate it.
  // Evaluating a symbol means finding its value,
  // if it is bound. An unbound symbol evaluates
  // to itself (its literal value).
  
  // Lookup symbol in global symbol table.
  if (IsSymbol(item))
  {
    Item value = ListGet(interp->symbolValueTable, item.value.symbol);
    if (TypeVirgin != value.type) 
      return value;
  }
  
  // Lookup symbol in stackframe local environment.
  if (IsLocalSymbol(item))
  {
    // Get current stackframe.
    List* stackframe = ItemList(ListGet(interp->callstack, interp->stackframeIndex));
    List* env = ItemList(ListGet(stackframe, StackFrameEnv));
    Item value = ListGet(env, item.value.symbol);
    if (TypeVirgin != value.type) 
      return value;
  }

  // Otherwise return the item itself (evaluate to itself).
  return item;
}

void InterpEval(Interp* interp, Item element)
{
  if (IsSymbol(element) || IsLocalSymbol(element))
  {
    // Evaluate symbol to see if it is bound to a function.
    Item value = InterpEvalSymbol(interp, element);
    
    // If primitive function, evaluate it.
    if (IsPrimFun(value))
    {
      PrintLine("PRIM FOUND: %i", element.value.symbol);
      // Call the primitive.
      value.value.primFun(interp);
      return;
    }
    
    if (IsFun(value))
    {
      PrintLine("FUN FOUND: %i", element.value.symbol);
      // Call the function.
      // TODO: InterpEvalFun(interp, item.value.fun);
      return;
    }
  }

  // Otherwise push element onto the data stack (not evaluated).
  ListPush(interp->stack, element);
  PrintDebug("PUSH ELEMENT ONTO DATA STACK TYPE: %u", element.type);
}

// TODO: Delete?
// Associative list
/*Item ListLookup(List* list, Index symbolIndex)
{
  // TODO
  return ItemWithVirgin();
}*/

void InterpPush(Interp* interp, Item item)
{
  ListPush(interp->stack, item);
}

Item InterpPop(Interp* interp)
{
  return ListPop(interp->stack);
}

Item InterpPopEval(Interp* interp)
{
  Item item = ListPop(interp->stack);
  return InterpEvalSymbol(interp, item);
}

void InterpPushStackFrame(Interp* interp, List* codeList)
{
  List* stackframe = StackFrameCreate(codeList);
  Item item = ItemWithList(stackframe);
  interp->stackframeIndex = ListPush(interp->callstack, item);
  PrintLine("PUSHED STACKFRAME AT INDEX: %i", interp->stackframeIndex);
  ListPrint(stackframe, interp);
}

void InterpRun(Interp* interp, List* list)
{
  // Push root stackframe.
  InterpPushStackFrame(interp, list);
  PrintDebug("CREATED ROOT FRAME AT INDEX: %i", interp->stackframeIndex);
  
  while (interp->run)
  {
    // Get current stackframe.
    //Item stackframe = ListGet(interp->callstack, interp->stackframeIndex);
    List* stackframe = ItemList(ListGet(interp->callstack, interp->stackframeIndex));

    //octo: that was new

    // Increment code pointer.
    Item codePointer = ListGet(stackframe, StackFrameCodePointer);
    codePointer.value.intNum ++;
    ListSet(stackframe, StackFrameCodePointer, codePointer);

    // If the code in the stackframe has finished executing
    // we exit the frame.
    List* codeList = ItemList(ListGet(stackframe, StackFrameCodeList));

    if (codePointer.value.intNum >= ListLength(codeList))
    {
      // EXIT STACK FRAME
      printf("EXIT STACKFRAME: %i\n", interp->stackframeIndex);
      interp->stackframeIndex--;
      StackFrameFree(stackframe);
      ListPop(interp->callstack);
    }
    else
    {
      // Evaluate the current element. Note that new stackframes
      // may be created during evaluation, and that this will 
      // increment the stackframe index.
      Item element = ListGet(codeList, codePointer.value.intNum);
      InterpEval(interp, element);
    }

    // Was this the last stackframe?
    if (interp->stackframeIndex < 0)
    {
      printf("EXIT InterpRun\n");
      interp->run = FALSE;
    }
  }
}

void InterpAddPrimFun(char* name, PrimFun fun, Interp* interp)
{
  // Add name to symbol table.
  ListPush(interp->symbolTable, ItemWithString(name));
  
  // Add function to symbol value table.
  ListPush(interp->symbolValueTable, ItemWithPrimFun(fun));
}

/***

Before compile:

((X) (A B) 
  (1 A SET 
   N A + B SET 
   B))

Lower case:

((x) (a b) 
  (1 a set
   x a + b set
   b))
   
Variation in style:

((x) (a b) 
  ((1 a set) do
   (x a + b set) do
   b))
   
   
StackFrame env becomes 0:X-value 1:A-value 2:B-value

After compile:

(1 (X A B)
  (1 (VAR 1) SET 
   (VAR 0) (VAR 1) + (VAR 2) SET 
   (VAR 2))) 

FUNINST
  FUN: NUMARGS VARLIST BODY
  ENV: ARRAY
  
The parser compiles functions.

Example:

((X) () (X X +)) FUN DOUBLE SET
21 DOUBLE PRINTLN

***/

int InterpCompileFunLookupLocalIndex(Interp* interp, List* localVars, Item symbol)
{
  for (int i = 0; i < ListLength(localVars); i++)
  {
    Item localSymbol = ListGet(localVars, i);
    if (ItemEquals(symbol, localSymbol)) 
    {
      return i;
    }
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
  PrintDebug("Compile Fun");
  
  if (!IsList(funList))
    ErrorExit("InterpCompileFun: funList is not a list");

  int length = ListLength(funList.value.list);

  if (3 != length)
    ErrorExit("InterpCompileFun: Wrong number of elements in funList");

  Item argList  = ListGet(funList.value.list, 0);
  Item varList  = ListGet(funList.value.list, 1);
  Item bodyList = ListGet(funList.value.list, 2);
  
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
  int numArgs = ListLength(argList.value.list);
  int numVars = ListLength(varList.value.list);

  for (int i = 0; i < numArgs; i++)
    ListPush(localVars, ListGet(argList.value.list, i));
  for (int i = 0; i < numVars; i++)
    ListPush(localVars, ListGet(varList.value.list, i));
  
  // Recursively traverse bodyList and replace local symbols with indexes.
  Item funBody = InterpCompileFunReplaceSymbols(interp, localVars, bodyList.value.list);
  
  // Create list for the compile function. TODO Struct?
  List* compiledFun = ListCreate();
  
  ListPush(compiledFun, ItemWithIntNum(numArgs));
  ListPush(compiledFun, ItemWithList(localVars));
  ListPush(compiledFun, funBody);
  
  // Return item with the compiled list.
  return ItemWithFun(compiledFun);
}

/* OLD VERSION
Item InterpCompileFun(Interp* interp, List* funList)
{
  int bodyIndex    = 0;
  int argListIndex = 0;
  int varListIndex = 0;
  int numArgs      = 0;
  
  int length = ListLength(funList);
  
  if (2 == length)
  {
    bodyIndex = 1;
  }
  else if (3 == length)
  {
    bodyIndex = 2;
    Item list = ListGet(funList, 1);
    if (!IsList(list))
    {
      ErrorExit("Second element not a list in InterpCompileFun");
    }
    Item first = ListGet(list, 0);
    if (!IsSymbol(first))
    {
      ErrorExit("First element not a symbol in InterpCompileFun");
    }
    char* string = InterpGetSymbolString(interp, first.value.symbol);
    if (StringEquals("VAR", string) || StringEquals("var", string))
    {
      varListIndex = 1;
    }
    else
    {
      argListIndex = 1;
    }
  }
  else if (4 == length)
  {
    // TODO: We should check that varlist begins with VAR.
    argListIndex = 1;
    varListIndex = 2;
  }
  else
  {
    ErrorExit("List has wrong length in InterpCompileFun");
  }
  
  if (argListIndex)
  {
    numArgs = ListLength(ListGet(funList, argListIndex));
  }
  
  List* localVars = ListCreate();
  if (argListIndex)
  {
    numArgs = ListLength(ListGet(funList, argListIndex));
  }
    
    HÄR ÄR JAG
    
  // TODO
  PrintDebug("Compile Fun");
  return ItemWithVirgin();
}
*/

// Bind stack parameters and push a stackframe with
// the function body.
void InterpEvalFun(List* fun, Interp* interp)
{
  // TODO

 // When binding params, set all localvars to TypeVirgin
}
