
// Function fields.
#define FunNumArgs             0
#define FunNumVars             1
#define FunBody                2

// Context environment handling.
#define ContextNoBorrowEnv     0
#define ContextBorrowEnv       1

// Declarations.
Item  InterpEvalSymbol(Interp* interp, Item item);
void  InterpEvalList(Interp* interp, List* list);
Item  InterpCompileFun(Interp* interp, Item funList);

// Octo: That was new

// With freedom comes responsibilities (for the programmer).
// Dynamic languages in action.

/***** CONTEXT *******************************************/

typedef struct MyContext
{
  List* code;
  List* env;
  Index codePointer;
}
Context;

Context* ContextCreate()
{
  Context* context = malloc(sizeof(Context));
  context->env = ListCreate();
  context->code = NULL;
  context->codePointer = -1;
  return context;
}

void ContextFree(Context* context)
{
  if (context->env)
    ListFree(context->env, ListFreeShallow);
  free(context);
}

#define ContextEnv(context) ((context)->env)

/***** INTERPRETER ***************************************/

typedef struct MyInterp
{
  // TODO: Make table for primfuns.
  List* symbolTable;      // List of global names
  List* symbolValueTable; // List of global values (primfuns, funs, etc)
  List* stack;            // The data stack
  List* callstack;        // Callstack with context frames
  int   callstackIndex;   // Index of current frame
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
  interp->callstackIndex = -1;
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

// Pop an item from the stack and set variable to it.
#define InterpPopSet(interp, item) ListPopSet((interp)->stack, item)

// Pop and evaluate an item from the stack and set variable to it.
#define InterpPopEvalSet(interp, item) \
  do { \
    ListPopSet((interp)->stack, item); \
    (item) = (IsSymbol(item) || IsLocalVar(item)) ? \
      InterpEvalSymbol(interp, item) : \
      item; \
  } while(0)

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
  ErrorExit("InterpSetLocalSymbolValue is not implemented!");
  /*
  List* env = InterpGetLocalEnv(interp);
  if (env)
    ListSet(env, index, value);
  else
    ErrorExit("InterpSetLocalSymbolValue: Local environment not found");
  */
}

char* InterpGetSymbolString(Interp* interp, Index symbolIndex)
{
  Item item = ListGet(interp->symbolTable, symbolIndex);
  return item.value.string;
}

/***** CONTEXT FRAMES ************************************/

// Return the context for the next call.
Context* InterpEnterContext(Interp* interp, List* code, int borrowEnv)
{
  List* callstack = interp->callstack;
  Index callstackIndex = interp->callstackIndex;

  // 1. Check for tail call optimization.
  if (interp->callstackIndex > -1)
  {
    Context* context = ItemContext(ListGet(callstack, callstackIndex));
    
    Index codePointer = context->codePointer;
    List* currentCodeList = context->code;

    // Check for tail call.
    if (codePointer + 1 >= ListLength(currentCodeList))
    {
      PrintDebug("TAILCALL AT INDEX: %i", interp->callstackIndex);
      // Tail call.
      context->code = code;
      context->codePointer = -1;
      if (!borrowEnv)
        context->env->length = 0;
      return context;
    }
  }

  // Increment stackframe index.
  callstackIndex ++;
  interp->callstackIndex = callstackIndex;

  // 2. Create new stackframe if needed.
  if (ListLength(callstack) <= callstackIndex)
  {
    PrintDebug("NEW CONTEXT AT INDEX: %i", callstackIndex);
    // Create stackframe.
    Context* context = ContextCreate();
    context->code = code;
    context->codePointer = -1;
    if (!borrowEnv)
      context->env->length = 0;
    ListSet(callstack, callstackIndex, ItemWithContext(context));
    return context;
  }

  // 3. Reuse existing stackframe.
  PrintDebug("ENTER CONTEXT AT INDEX: %i", callstackIndex);
  Context* context = ItemContext(ListGet(callstack, callstackIndex));
  context->code = code;
  context->codePointer = -1;
  if (!borrowEnv)
    context->env->length = 0;
  return context;
}

void InterpExitContext(Interp* interp)
{
  PrintDebug("EXIT CONTEXT: %i", interp->callstackIndex);

  //Context* context = ItemContext(ListPop(interp->callstack));
  //ContextFree(stackframe);

  interp->callstackIndex--;
}

/***** SYMBOL TABLE **************************************/

// Lookup the symbol string in the symbol table.
Index InterpLookupSymbolIndex(Interp* interp, char* symbolString)
{
  List* symbolTable = interp->symbolTable;
  for (int i = 0; i < ListLength(symbolTable); i++)
  {
    Item item = ListGet(symbolTable, i);
    char* string = item.value.string;
    if (StringEquals(string, symbolString))
    {
      // Found it.
      return i;
    }
  }
  return -1; // Not found.
}

// Add a symbol to the symbol table and return an
// item with the entry. Used by the parser.
Item InterpAddSymbol(Interp* interp, char* symbolString)
{
  // Lookup the symbol.
  Index index = InterpLookupSymbolIndex(interp, symbolString);
  if (index > -1)
  {
    // Symbol is already added, return an item for it.

    //PrintDebug("SYMBOL EXISTS IN SYMTABLE: %s\n", symbol);

    // Special case for primfuns. We return the primfun item.
    /*Item value = ListGet(interp->symbolValueTable, index);
    //PrintDebug("InterpAddSymbol: %s", symbolString);
    if (IsPrimFun(value))
    {
      //PrintDebug("InterpAddSymbol Symbol is PrimFun: %s", symbolString);  
      return value;
    }*/

    // It was not a primfun, return a symbol item.
    Item item = ItemWithSymbol(index);
    return item;
  }
  else
  {
    // Symbol does not exist, create it.
    Item newItem = ItemWithString(symbolString);
    ListPush(interp->symbolTable, newItem);
    ListPush(interp->symbolValueTable, ItemWithVirgin());
    Index newIndex = ListLength(interp->symbolTable) - 1;
    // Make a symbol item and return it.
    Item item = ItemWithSymbol(newIndex);
    return item;
  }
}

/***** EVAL SYMBOL ***************************************/

// Lookup the value of a symbol (variable value).
// If the item is a symbol, evaluate it.
// Return Virgin item if no value exists.
// Evaluating a symbol means finding its value,
// if it is bound. An unbound symbol evaluates
// to itself (its literal value).
Item InterpEvalSymbol(Interp* interp, Item item)
{
  // Lookup symbol in stackframe local environment.
  if (IsLocalVar(item))
  {
    Context* context;
    List* env;
    List* callstack = interp->callstack;
    Index callstackIndex = interp->callstackIndex;
    while (callstackIndex > -1)
    {
      context = ItemContext(ListGet(callstack, callstackIndex));
      env = context->env;

      //PrintLine("CONTEXT in InterpEvalSymbol:");
      //ListPrint(env, interp);

      // Search environment.
      if (ListLength(env) > 0)
        return ListGet(env, item.value.symbol);
      callstackIndex--;
    }
  }

  // Lookup symbol in global symbol table.
  if (IsSymbol(item))
  {
    Item value = ListGet(interp->symbolValueTable, item.value.symbol);
    if (!IsVirgin(value))
      return value;
  }

  // Otherwise return the item itself (evaluate to itself).
  return item;
}

/****************** EVAL FUNCTION ******************/

// Bind stack parameters and push a context on the
// callstack with the function body.
void InterpEvalFun(Interp* interp, List* fun)
{
  // Get info for the call context of the function.
  IntNum numArgs = ItemIntNum(ListGet(fun, FunNumArgs));
  IntNum numVars = ItemIntNum(ListGet(fun, FunNumVars));
  List* funBody = ItemList(ListGet(fun, FunBody));

  // Get context for the function call. 
  Context* context = InterpEnterContext(interp, funBody, ContextNoBorrowEnv);

  // Get environment.
  List* env = context->env;

  // Set it to empty.
  env->length = 0;

  // Bind parameters.
  for (int i = numArgs - 1; i >= 0; i--)
  {
    //Item arg = InterpPopEval(interp);
    Item arg;
    InterpPopEvalSet(interp, arg);
    ListSet(env, i, arg);
  }
/*
  // Set all localvars to TypeVirgin.
  for (int i = numArgs; i < numArgs + numVars; i++)
  {
    ListSet(env, i, ItemWithVirgin());
  }
*/
}

/**** EVAL LIST ******************************************/

void InterpEvalList(Interp* interp, List* code)
{
  InterpEnterContext(interp, code, ContextBorrowEnv);
}

/**** MAIN INTERPRETER LOOP ******************************/

// TODO: Begin Optimizing
void InterpRun(Interp* interp, List* list)
{
  // Create root context.
  InterpEvalList(interp, list);

  PrintDebug("CREATED ROOT FRAME AT INDEX: %i", interp->callstackIndex);
  
  //ListPrint(list, interp);
    
  while (interp->run)
  {
    // Get current context.
    Context* context = ItemContext(ListGet(interp->callstack, interp->callstackIndex));

    // Increment code pointer.
    context->codePointer ++;

    // If the code in the current context has finished executing
    // we exit the frame.
    List* code = context->code;

    if (context->codePointer >= ListLength(code))
    {
      // EXIT Context
      InterpExitContext(interp);
      //interp->callstackIndex--;
    }
    else
    {
      // Evaluate the current element. Note that new stackframes
      // may be created during evaluation, and that this will 
      // increment the stackframe index.
      Item element = ListGet(code, context->codePointer);

      PrintDebug("EVAL GOT ELEMENT");
      //ItemPrint(element, interp);

      // If the elements points directly to a primful we run it.
      if (IsPrimFun(element))
      {
        PrintDebug("CALLING PRIMFUN");
        element.value.primFun(interp);
        goto exit;
      }

      // Get value from local environment.
      if (IsLocalVar(element))
      {
        Item value = InterpEvalSymbol(interp, element);
        if (IsFun(value))
        {
          InterpEvalFun(interp, ItemList(value));
          goto exit;
        }
      }

  PrintDebug("P3");
      // If a symbol we check the bound value can be evaluated (primfun or fun).
      if (IsSymbol(element))
      {
  PrintDebug("P31");
        // Here we get the global value and check for functions
        // and primitives.
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

      // Otherwise
      PrintDebug("PUSHING ON STACK: %lu", element.type);

      ListPush(interp->stack, element);

      PrintDebug("PRINTING STACK:");
      //ListPrintItems(interp->stack, interp);
    }
exit:
    // Was this the last stackframe?
    if (interp->callstackIndex < 0)
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
/*
Ways to lookup variables:

Get values from the stack and dulicate and shuffle around as needed (Forth)
Lookup in a hashtable with ints or strings O(logn)
Lookup in a tree O(logn)
Lookup in an array by index (precompiled indexes) O(1)
Lookup in an array with binary search (array must be ordered) O(logn)
Lookup in an array with linear search O(n)
*/
/*
Item
  Context
    List
    Env
    ParentEnv
    instrPointer
*/

/****************** "COMPILE" ******************/

int InterpCompileFunLookupLocalIndex(Interp* interp, List* localVars, Item symbol)
{
  for (int i = 0; i < ListLength(localVars); i++)
  {
    Item localSymbol = ListGet(localVars, i);
    if (symbol.value.symbol == localSymbol.value.symbol)
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

