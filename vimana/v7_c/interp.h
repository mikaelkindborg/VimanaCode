
// DECLARATIONS ------------------------------------------------

// Case of primfun symbols
#define SymbolUpperCase     1
#define SymbolLowerCase     2
#define SymbolMixedCase     3

// Declarations.
//void PrimEval_EvalList(Interp* interp, List* list);
//void PrimEval_EvalFun(Interp* interp, List* fun);
Item PrimEval_EvalSymbol(Interp* interp, Item item);

// Octo: That was new

// With freedom comes responsibilities (for the programmer).
// Dynamic languages in action.

// CONTEXT -----------------------------------------------------

typedef struct MyContext
{
  List* env;
  List* ownEnv;
  //Bool  hasEnv;
  List* code;
  Index codePointer;
  struct MyContext* nextContext;
  struct MyContext* prevContext;
}
Context;

Context* ContextCreate()
{
  Context* context = malloc(sizeof(Context));
  context->ownEnv = ListCreate();
  context->env = NULL;
  context->code = NULL;
  context->codePointer = -1;
  context->nextContext = NULL;
  context->prevContext = NULL;
  return context;
}

void ContextFree(Context* context)
{
  ListFree(context->ownEnv, ListFreeShallow);
  free(context);
}

// INTERPRETER -------------------------------------------------

typedef struct MyInterp
{
  List*    globalSymbolTable; // List of symbols
  List*    globalValueTable;  // List of global values (primfuns, funs, etc)
  List*    stack;             // The data stack
  Context* callstack;         // Callstack with context frames
  Context* currentContext;    // Currently executing context
  int      callstackIndex;    // Index of current frame
  Bool     run;               // Run flag
  int      symbolCase;        // Casing for primitive functions
  Bool     contextSwitch;     // Context switching flag
}
Interp;

Interp* InterpCreate()
{
  Interp* interp = malloc(sizeof(Interp));
  interp->globalSymbolTable = ListCreate();
  interp->globalValueTable = ListCreate();
  interp->stack = ListCreate();
  interp->symbolCase = SymbolUpperCase;
  return interp;
}

void InterpFree(Interp* interp)
{
  ListFree(interp->globalSymbolTable, ListFreeDeep);
  ListFree(interp->globalValueTable, ListFreeShallow);
  ListFree(interp->stack, ListFreeShallow);
  //TODO Free contexts //ListFree(interp->callstack, ListFreeDeep);
  free(interp);
}

// DATA STACK --------------------------------------------------

// Push an item onto the data stack.
#define InterpPush(interp, item) ListPush((interp)->stack, item)

// Pop an item from the data stack and set variable to it.
#define InterpPopInto(interp, item) ListPopInto((interp)->stack, item)
#define InterpPopEvalInto(interp, item) ListPopInto((interp)->stack, item)

// Pop and evaluate an item from the stack and set variable to it.
/*
#define InterpPopEvalInto(interp, item) \
  do { \
    ListPopInto((interp)->stack, item); \
    (item) = IsSymbol(item) ? \
      PrimEval_EvalSymbol(interp, item) : \
      item; \
  } while (0)
*/

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
      return value;
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

void InterpAddPrimFun(char* str, PrimFun fun, Interp* interp)
{
  // TODO: Free these strings in InterpFree().
  char* name = malloc(strlen(str) + 1);
  strcpy(name, str);

  // Symbols for primitives are in mixed case.
  // If you want to use lower or upper case, you can
  // specify that in the interpreter object.
  if (SymbolUpperCase == interp->symbolCase)
    StringToUpper(name);
  else if (SymbolLowerCase == interp->symbolCase)
    StringToLower(name);

  // Add name to symbol table.
  ListPush(interp->globalSymbolTable, ItemWithString(name));
  
  // Add function to value table.
  Item item;
  item.type = TypePrimFun;
  item.value.primFun = fun;
  ListPush(interp->globalValueTable, item);
}

// CALLSTACK ---------------------------------------------------

#define InterpEnterContext(interp, code) \
  InterpEnterCallContext(interp, code, FALSE)

void InterpEnterCallContext(Interp* interp, List* code, Bool isFunCall)
{
  Context* currentContext;
  Context* nextContext;

  interp->contextSwitch = TRUE;

  // Obtain current context.
  currentContext = interp->currentContext;

#ifndef OPTIMIZE
  // Check for NULL context.
  if (!currentContext)
    ErrorExit("InterpEnterCallContext: currentContext is NULL");
#endif

  // Check for possible tailcall.
  if (currentContext->codePointer + 1 >= ListLength(currentContext->code))
  {
    // We can do a tailcall and reuse the current context.
    PrintDebug("TAILCALL AT INDEX: %i", interp->callstackIndex);
    nextContext = currentContext;
  }
  else
  {
#ifndef OPTIMIZE
    // Increment callstack index.
    ++ interp->callstackIndex;
#endif

    // Create new context or reuse existing context.
    if (NULL == currentContext->nextContext)
    {
      PrintDebug("NEW CONTEXT AT INDEX: %i", interp->callstackIndex);
      nextContext = ContextCreate();
      nextContext->prevContext = currentContext; 
      currentContext->nextContext = nextContext;
    }
    else
    {
      PrintDebug("REUSE CONTEXT AT INDEX: %i", interp->callstackIndex);
      nextContext = currentContext->nextContext;
    }
  }

  // Set context data.
  nextContext->code = code; // TODO: What about GC?
  nextContext->codePointer = -1;

  // Set environment.
  if (isFunCall)
  {
    // Use fresh environment.
    nextContext->env = nextContext->ownEnv;
    nextContext->env->length = 0;
  }
  else if (code->env)
  {
    // Use closure environment.
    nextContext->env = code->env;
  }
  else
  {
    // Use environment of previous context.
    nextContext->env = currentContext->env;
  }

  interp->currentContext = nextContext;
}

// Evaluate list.
void InterpRun(Interp* interp, List* list)
{
  Context*   currentContext;
  List*      code;
  int        codePointer;
  int        codeLength;
  Item       element;
  Item       item;

  // Create root context.
  interp->callstack = ContextCreate();
  interp->currentContext = interp->callstack;
  interp->callstackIndex = 0;
  interp->run = TRUE;
  interp->currentContext->code = list;
  interp->contextSwitch = TRUE;

  while (TRUE) //interp->run
  {
    // Check context switch.
    if (interp->contextSwitch)
    {
      interp->contextSwitch = FALSE;
      currentContext = interp->currentContext;
      code = currentContext->code;
      codeLength = ListLength(code);
    }

    // Increment code pointer.
    codePointer = ++ currentContext->codePointer;

    // Exit the frame if the code in the current context has finished executing.
    if (codePointer >= codeLength)
    {
      PrintDebug("EXIT CONTEXT: %i", interp->callstackIndex);
      interp->currentContext = interp->currentContext->prevContext;
      interp->contextSwitch = TRUE;
#ifndef OPTIMIZE
      -- interp->callstackIndex;
      if (interp->currentContext)
      {
        ContextFree(interp->currentContext->nextContext);
        interp->currentContext->nextContext = NULL;
      }
#endif
      goto exit;
    }

    // Get next element.
    Item element = ListGet(code, codePointer);

#ifdef OPTIMIZE
    // Optimized primfun calls
    if (IsPrimFun(element))
    {
      element.value.primFun(interp);
      goto exit;
    }
#endif

    if (IsSymbol(element))
    {
/*
#ifndef OPTIMIZE
      // Non-optimized primfun calls.
      // Lookup global value of symbol and check if it is a primfun.
      item = ListGet(interp->globalValueTable, element.value.symbol);
      if (IsPrimFun(item))
      {
        item.value.primFun(interp);
        goto exit;
      }
#endif
*/
      // Evaluate symbol (search local and global env).
      item = PrimEval_EvalSymbol(interp, element);
      
#ifndef OPTIMIZE      
      if (IsPrimFun(item))
      {
        item.value.primFun(interp);
        goto exit;
      }
#endif
      // If it is a function call it.
      if (IsFun(item))
      {
        InterpEnterCallContext(interp, item.value.list, TRUE);
        goto exit;
      }

      // If not a function, evaluate symbol and push the value.
      ListPush(interp->stack, item);
      goto exit;
    }

    // If none of the above, push the element.
    ListPush(interp->stack, element);

exit:
    // Was this the last stackframe?
    if (NULL == interp->currentContext)
    //if (interp->callstackIndex < 0)
    {
      PrintDebug("EXIT InterpRun callstackIndex: %i", interp->callstackIndex);
      //interp->run = FALSE;
      break;
    }
  } // while
}
