
// StackFrame fields.
#define StackFrameCodeList     0
#define StackFrameCodePointer  1
#define StackFrameHasEnv       2
#define StackFrameEnv          3

// Function fields.
#define FunNumArgs             0
#define FunNumVars             1
#define FunBody                2

// Declarations.
Item InterpEvalSymbol(Interp* interp, Item item);
Item InterpCompileFun(Interp* interp, Item funList);
List* InterpGetLocalEnv(Interp* interp);

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
  // TODO: Do custom free functions for each type.
  ListFree(interp->symbolTable, ListFreeDeep);
  ListFree(interp->symbolValueTable, ListFreeShallow);
  ListFree(interp->stack, ListFreeShallow);
  ListFree(interp->callstack, ListFreeDeep);
}

// Push item on the data stack.
void InterpPush(Interp* interp, Item item)
{
  ListPush(interp->stack, item);
}

// Pop item offf the data stack.
Item InterpPop(Interp* interp)
{
  return ListPop(interp->stack);
}

// Pop item off the data stack and evaluate it
// if it is a symbol.
Item InterpPopEval(Interp* interp)
{
  Item item = ListPop(interp->stack);
  return InterpEvalSymbol(interp, item);
}

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

// Invocation object/activation frame

// Experiment using a list to represent a stackframe, 
// rather than a struct.
List* StackFrameCreate(List* codeList, List* env)
{
  List* stackframe = ListCreate();

  ListSet(stackframe, StackFrameCodeList, ItemWithList(codeList));
  ListSet(stackframe, StackFrameCodePointer, ItemWithIntNum(-1));

  if (env)
  {
    ListSet(stackframe, StackFrameHasEnv, ItemWithBool(TRUE));
    ListSet(stackframe, StackFrameEnv, ItemWithList(env));
  }
  else
  {
    ListSet(stackframe, StackFrameHasEnv, ItemWithBool(FALSE));
  }

  return stackframe;
}

void StackFrameFree(List* stackframe)
{
  Bool hasEnv = ItemBool(ListGet(stackframe, StackFrameHasEnv));
  if (hasEnv)
  {
    List* env = ItemList(ListGet(stackframe, StackFrameEnv));
    ListFree(env, ListFreeShallow);
  }
  ListFree(stackframe, ListFreeShallow);
}

void InterpPushStackFrame(Interp* interp, List* codeList, List* env)
{
  List* stackframe = StackFrameCreate(codeList, env);
  Item item = ItemWithList(stackframe);
  interp->stackframeIndex = ListPush(interp->callstack, item);
  PrintLine("PUSHED STACKFRAME AT INDEX: %i", interp->stackframeIndex);
  ListPrint(stackframe, interp);
}

void InterpPopStackFrame(Interp* interp)
{
  PrintDebug("EXIT STACKFRAME: %i", interp->stackframeIndex);
  List* stackframe = ItemList(ListPop(interp->callstack));
  interp->stackframeIndex--;
  StackFrameFree(stackframe);
}

List* InterpGetLocalEnv(Interp* interp)
{
  List* stackframe;
  Bool hasEnv;
  Index stackFrameIndex = interp->stackframeIndex;
  while (stackFrameIndex > 0)
  {
    stackframe = ItemList(ListGet(interp->callstack, stackFrameIndex));
    hasEnv = ItemBool(ListGet(stackframe, StackFrameHasEnv));
    if (hasEnv)
      return ItemList(ListGet(stackframe, StackFrameEnv));
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

  // Create environment.
  List* env = ListCreate();
  //ListSet(env, 0, ItemWithIntNum(42));
  //ListSet(env, 0, ItemWithList(fun));

  // Bind parameters.
  for (int i = 0; i < numArgs; i++)
  {
    Item arg = InterpPopEval(interp);
    ListPush(env, arg);
  }

  // Set all localvars to TypeVirgin.
  for (int i = 0; i < numVars; i++)
  {
    ListPush(env, ItemWithVirgin());
  }

  InterpPushStackFrame(interp, funBody, env);
}

/****************** EVAL ELEMENT ******************/

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
      InterpEvalFun(interp, ItemList(value));
      return;
    }
  }

  // Otherwise push element onto the data stack (not evaluated).
  ListPush(interp->stack, element);
  PrintDebug("PUSH ELEMENT ONTO DATA STACK TYPE: %u", element.type);
}

/****************** MAIN INTERPRETER LOOP ******************/

void InterpRun(Interp* interp, List* list)
{
  // Push root stackframe.
  InterpPushStackFrame(interp, list, NULL);
  PrintDebug("CREATED ROOT FRAME AT INDEX: %i", interp->stackframeIndex);
  
  while (interp->run)
  {
    // Get current stackframe.
    // TODO: Optimize this?
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
      InterpPopStackFrame(interp);
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
      PrintDebug("EXIT InterpRun");
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
  PrintDebug("Compile Fun");
  
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

  PrintDebug("COMPILED FUNCTION:");
  ListPrint(compiledFun, interp);
  
  // Return item with the compiled list.
  return ItemWithFun(compiledFun);
}
