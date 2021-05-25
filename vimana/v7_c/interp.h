
// DECLARATIONS ------------------------------------------------

// Context environment handling.
#define ContextNewEnv       1
#define ContextCurrentEnv   0

// Declarations.
//void PrimEval_EvalList(Interp* interp, List* list);
//void PrimEval_EvalFun(Interp* interp, List* fun);
Item PrimEval_EvalSymbol(Interp* interp, Item item);

// Octo: That was new

// With freedom comes responsibilities (for the programmer).
// Dynamic languages in action.

// INTERPRETER -------------------------------------------------

typedef struct MyInterp
{
  List*    globalSymbolTable; // List of symbols
  List*    globalValueTable;  // List of global values (primfuns, funs, etc)
  List*    stack;             // The data stack
  List*    callstack;         // Callstack with context frames
  int      callstackIndex;    // Index of current frame
  Context* currentContext;    // Currently executing context
  Bool     run;               // Run flag
}
Interp;

Interp* InterpCreate()
{
  Interp* interp = malloc(sizeof(Interp));
  interp->globalSymbolTable = ListCreate();
  interp->globalValueTable = ListCreate();
  interp->stack = ListCreate();
  interp->callstack = ListCreate();
  interp->callstackIndex = -1;
  interp->currentContext = NULL;
  interp->run = TRUE;
  return interp;
}

void InterpFree(Interp* interp)
{
  ListFree(interp->globalSymbolTable, ListFreeDeep);
  ListFree(interp->globalValueTable, ListFreeShallow);
  ListFree(interp->stack, ListFreeShallow);
  ListFree(interp->callstack, ListFreeDeep);
  free(interp);
}

// CONTEXT -----------------------------------------------------

typedef struct MyContext
{
  Bool  hasEnv;
  List* env;
  List* code;
  Index codePointer;
  struct MyContext* prevContext;
}
Context;

Context* ContextCreate()
{
  Context* context = malloc(sizeof(Context));
  context->env = ListCreate();
  context->hasEnv = TRUE;
  context->code = NULL;
  context->codePointer = -1;
  context->prevContext = NULL;
  return context;
}

void ContextFree(Context* context)
{
  ListFree(context->env, ListFreeShallow);
  free(context);
}

#ifdef OPTIMIZE
#define ContextInitEnv(context, newEnv) \
  do { \
    if (newEnv) { \
      context->env->length = 0; \
      context->hasEnv = TRUE; } \
    else \
      context->hasEnv = FALSE; \
  } while(0)
#else
void ContextInitEnv(Context* context, int newEnv)
{
  if (newEnv) 
  {
    context->env->length = 0;
    context->hasEnv = TRUE;
  }
  else
  {
    context->hasEnv = FALSE;
  }
}
#endif

// DATA STACK --------------------------------------------------

// Push an item onto the data stack.
#define InterpPush(interp, item) ListPush((interp)->stack, item)

// Pop an item from the data stack and set variable to it.
#define InterpPopInto(interp, item) ListPopInto((interp)->stack, item)

// Pop and evaluate an item from the stack and set variable to it.
#define InterpPopEvalInto(interp, item) \
  do { \
    ListPopInto((interp)->stack, item); \
    (item) = IsSymbol(item) ? \
      PrimEval_EvalSymbol(interp, item) : \
      item; \
  } while(0)

// SYMBOL TABLE ------------------------------------------------

void InterpSetGlobalSymbolValue(Interp* interp, Index index, Item value)
{
  ListSet(interp->globalValueTable, index, value);
}

char* InterpGetSymbolString(Interp* interp, Index symbolIndex)
{
  Item item = ListGet(interp->globalSymbolTable, symbolIndex);
  return item.value.string;
}

// Lookup a string in the global symbol table and return
// the index if it is found.
Index InterpLookupSymbolIndex(Interp* interp, char* symbolString)
{
  List* symbolTable = interp->globalSymbolTable;
  for (int i = 0; i < ListLength(symbolTable); ++ i)
  {
    Item item = ListGet(symbolTable, i);
    char* string = item.value.string;
    if (StringEquals(string, symbolString))
      return i; // Found it.
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
#ifdef OPTIMIZE
    // Special case for primfuns. We return the primfun item
    // for faster lookup in eval.
    Item value = ListGet(interp->globalValueTable, index);
    if (IsPrimFun(value))
    {
      return value;
    }
#endif
    // Symbol is already added, return an item for it.
    Item item = ItemWithSymbol(index);
    return item;
  }
  else
  {
    // Symbol does not exist, create it.
    Item newItem = ItemWithString(symbolString);
    ListPush(interp->globalSymbolTable, newItem);
    ListPush(interp->globalValueTable, ItemWithVirgin());
    Index newIndex = ListLength(interp->globalSymbolTable) - 1;
    // Make a symbol item and return it.
    Item item = ItemWithSymbol(newIndex);
    return item;
  }
}

void InterpAddPrimFun(char* name, PrimFun fun, Interp* interp)
{
  // Add name to symbol table.
  ListPush(interp->globalSymbolTable, ItemWithString(name));
  
  // Add function to value table.
  Item item;
  item.type = TypePrimFun;
  item.value.primFun = fun;
  ListPush(interp->globalValueTable, item);
}

// CALLSTACK ---------------------------------------------------

void InterpEnterContext(Interp* interp, List* code, int newEnv)
{
  List* callstack = interp->callstack;
  Index callstackIndex = interp->callstackIndex;
  Context* currentContext = NULL;
  interp->currentContext = NULL;

  // Remember current context.
  if (callstackIndex > -1)
    currentContext = ItemContext(ListGet(callstack, callstackIndex));

  // 1. Check for tail call optimization.
  if (callstackIndex > -1)
  {
    Index codePointer = currentContext->codePointer;
    List* currentCodeList = currentContext->code;

    // Check for tail call.
    if (codePointer + 1 >= ListLength(currentCodeList))
    {
      PrintDebug("TAILCALL AT INDEX: %i", callstackIndex);
      // Tail call.
      currentContext->code = code;
      currentContext->codePointer = -1;
      // For tailcalls we don't init the environment, but keep it as it is!
      //ContextInitEnv(currentContext, newEnv);
      return;
    }
  }

  // Increment stackframe index.
  ++ callstackIndex;
  interp->callstackIndex = callstackIndex;

  // 2. Create new stackframe if needed.
  if (ListLength(callstack) <= callstackIndex)
  {
    PrintDebug("NEW CONTEXT AT INDEX: %i", callstackIndex);
    interp->currentContext = NULL;
    Context* newContext = ContextCreate(); // Create stackframe
    newContext->code = code;
    newContext->codePointer = -1;
    ContextInitEnv(newContext, newEnv);
    newContext->prevContext = currentContext;
    ListSet(callstack, callstackIndex, ItemWithContext(newContext));
    return;
  }

  // 3. Reuse existing stackframe.
  PrintDebug("ENTER CONTEXT AT INDEX: %i", callstackIndex);
  interp->currentContext = NULL;
  Context* newContext = ItemContext(ListGet(callstack, callstackIndex));
  newContext->code = code;
  newContext->codePointer = -1;
  ContextInitEnv(newContext, newEnv);
  //newContext->prevContext = currentContext;
}

void InterpExitContext(Interp* interp)
{
  PrintDebug("EXIT CONTEXT: %i", interp->callstackIndex);
  interp->currentContext = NULL;
  //interp->currentContext = interp->currentContext->prevContext;
  -- interp->callstackIndex;
  Context* context = ItemContext(ListPop(interp->callstack));
  ContextFree(context);
}

// MAIN INTERPRETER LOOP ---------------------------------------

// Evaluate the list.
void InterpRun(Interp* interp, List* list)
{
  Context* context;
  List* code;
  int codePointer;
  Item item;

  // Create root context.
  InterpEnterContext(interp, list, ContextNewEnv);

  while (interp->run)
  { 
    // Get current context.
    if (NULL == interp->currentContext)
    {
      interp->currentContext = ItemContext(ListGet(interp->callstack, interp->callstackIndex));
      context = interp->currentContext;
      code = context->code;
    }
    //context = interp->currentContext;

    //code = context->code;
    // Increment code pointer.
    codePointer = ++ context->codePointer;

    //PrintDebug("InterpRun: codePointer: %i", codePointer);

    // Exit the frame if the code in the current context has finished executing.
    //code = context->code;
    if (codePointer >= ListLength(code))
    {
#ifdef OPTIMIZE
      //interp->currentContext = interp->currentContext->prevContext;
      interp->currentContext = NULL;
      -- interp->callstackIndex;
#else
      InterpExitContext(interp);
#endif
      goto exit;
    }

    // Get the next element.
    Item element = ListGet(code, codePointer);
    //PrintDebug("InterpRun: element type: %lu", element.type);

#ifdef OPTIMIZE
    // If the elements points directly to a primfun we run it.
    if (IsPrimFun(element))
    {
      //PrintDebug("CALLING PRIMFUN");
      element.value.primFun(interp);
      goto exit;
    }
#endif

    // If symbol we check if the global value is primfun or fun.
    // This means that functions defined with DEF will take 
    // precedence over local variables with the same name. 
    // Furthermore, local functions need to be called with CALL.
    if (IsSymbol(element))
    {
      item = ListGet(interp->globalValueTable, element.value.symbol);
      if (IsPrimFun(item))
      {
        item.value.primFun(interp);
        goto exit;
      }
      if (IsFun(item))
      {
        //PrimEval_EvalFun(interp, item.value.list);
        InterpEnterContext(interp, item.value.list, ContextNewEnv);
        goto exit;
      }
    }

    // If not primfun or function we push the element onto the data stack.
    ListPush(interp->stack, element);

exit:
    // Was this the last stackframe?
    if (interp->callstackIndex < 0)
    {
      //PrintDebug("EXIT InterpRun");
      interp->run = FALSE;
    }
  } // while
}
