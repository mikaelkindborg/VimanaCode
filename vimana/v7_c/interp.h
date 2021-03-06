
// DECLARATIONS ------------------------------------------------

// Case of primfun symbols
#define SymbolUpperCase     1
#define SymbolLowerCase     2
#define SymbolMixedCase     3

// Octo: That was new

// CONTEXT -----------------------------------------------------

typedef struct MyContext
{
  List* code;        // Code list
  Index codePointer; // Pointer in code list
  List* env;         // Environment used by the context
  List* ownEnv;      // Environment owned and reused by the context
  Bool isFunCall;    // Flag for function calls
  //Bool  releaseEnv;  // Release ownEnv on stackframe exit
  struct MyContext* nextContext;
  struct MyContext* prevContext;
}
Context;

// INTERPRETER -------------------------------------------------

// The symbol table and the global var table have the same
// length. This wastes some memory since not all symbols
// have global values. It also slows down GC a bit, because
// the entire gvar table is traversed on GC (except for primfuns,
// which appear first in the table, and can be skipped).
//
// There are speed benefits to this setup since direct lookup
// by index can be used for global vars. This scheme is intended
// as a simplistic and straightforward implementation.

typedef struct MyInterp
{
  List*    symbolTable;       // List of symbols
  List*    gvarTable;         // List of global variable values
  List*    stack;             // The data stack
  Context* callstack;         // Callstack with context frames
  Context* currentContext;    // Currently executing context
  int      callstackIndex;    // Index of current frame
  Bool     run;               // Run flag
  int      symbolCase;        // Casing for primitive functions
  Bool     contextSwitch;     // Context switching flag
  int      numberOfPrimFuns;  // First entries in symbol table
#ifdef USE_GC
  GarbageCollector* gc;
#endif
}
Interp;

// CREATE/FREE FUNCTIONS ---------------------------------------

// Create (garbage collected) list.
List* InterpListCreate(Interp* interp)
{
#ifdef USE_GC
  return GCListCreate(interp->gc);
#else
  return ListCreate();
#endif
}

// Create (garbage collected) list item.
Item InterpListItemCreate(Interp* interp)
{
  Item item;
  item.type = TypeList | TypeDynAlloc;
  item.value.list = InterpListCreate(interp);
  return item;
}

Context* ContextCreate(Interp* interp)
{
  Context* context = MemAlloc(sizeof(Context));
  context->ownEnv = InterpListCreate(interp);
  context->env = context->ownEnv;
  //context->releaseEnv = FALSE;
  context->isFunCall = FALSE;
  context->code = NULL;
  context->codePointer = -1;
  context->nextContext = NULL;
  context->prevContext = NULL;
  return context;
}

void ContextFree(Context* context)
{
  //PrintDebug("ContextFree ownEnv: %lu", (unsigned long)(context->ownEnv));
#ifndef USE_GC // ! USE_GC
  if (!context->ownEnv->isShared)
    ListFree(context->ownEnv);
#endif
  MemFree(context);
}

Interp* InterpCreate()
{
  Interp* interp = MemAlloc(sizeof(Interp));
#ifdef USE_GC
  interp->gc = GCCreate();
#endif
  interp->symbolTable = ListCreate();
  interp->gvarTable = ListCreate();
  interp->stack = ListCreate();
  interp->callstack = ContextCreate(interp);
  interp->symbolCase = SymbolUpperCase; // Default
  interp->numberOfPrimFuns = 0;
  return interp;
}

void InterpFree(Interp* interp)
{
  // Free strings in symbolTable.
  for (int i = 0; i < ListLength(interp->symbolTable); ++i)
  {
    Item item = ListGet(interp->symbolTable, i);
    StringFree(item.value.string);
  }

  // Free global tables and data stack.
  ListFree(interp->symbolTable);
  ListFree(interp->gvarTable);
  ListFree(interp->stack);

  // Free callstack.
  Context* context = interp->callstack;
  while (context)
  {
    Context* freeMe = context;
    context = context->nextContext;
    ContextFree(freeMe);
  }

#ifdef USE_GC
  // Free garbage collector (frees allocated objects).
  GCFree(interp->gc);
#endif

  // Free interpreter struct.
  MemFree(interp);
}

// GARBAGE COLLECTION ------------------------------------------

#ifdef USE_GC
void InterpGC(Interp* interp)
{
  // Debug printing.
  GCPrintEntries(interp->gc);

  // Mark items on the stack.
  GCMarkChildren(interp->stack, 0);

  // Mark global variables (skipping primfun entries).
  GCMarkChildren(interp->gvarTable, interp->numberOfPrimFuns);

  // Mark objects in active callstack contexts.
  Context* context = interp->currentContext;
  while (context)
  {
    GCMarkList(context->ownEnv);
    context = context->prevContext;
  }

  // Free unreachable objects.
  GCSweep(interp->gc);

  // Debug printing.
  GCPrintEntries(interp->gc);
}
#endif

// DATA STACK --------------------------------------------------

// Push an item onto the data stack.
#define InterpPush(interp, item) ListPush((interp)->stack, item)

// Pop an item from the data stack and set variable to it.
#define InterpPopInto(interp, item) ListPopInto((interp)->stack, item)

// SYMBOL TABLE ------------------------------------------------

char* InterpGetSymbolString(Interp* interp, Index symbolIndex)
{
  Item item = ListGet(interp->symbolTable, symbolIndex);
  return StringStr(item.value.string);
}

// Add a symbol to the symbol table and return an
// item with the entry. Used by the parser.
Item InterpAddSymbol(Interp* interp, char* symbolString)
{
  // Lookup primfun symbol.
  Index index = ListLookupStringIndex(interp->symbolTable, symbolString);
  if (index > -1)
  {
#ifdef OPTIMIZE_PRIMFUNS
    // Special case for primfuns. We return the primfun item
    // for faster lookup in eval.
    Item value = ListGet(interp->gvarTable, index);
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
    ListPush(interp->symbolTable, newItem);
    ListPush(interp->gvarTable, ItemWithVirgin());
    Index newIndex = ListLength(interp->symbolTable) - 1;
    // Make a symbol item and return it.
    Item item = ItemWithSymbol(newIndex);
    return item;
  }
}

void InterpAddPrimFun(char* str, PrimFun fun, Interp* interp)
{
  ++ interp->numberOfPrimFuns;

  // Name for primfun, case may be converted.
  char* name = MemAlloc(strlen(str) + 1);
  strcpy(name, str);

  // Symbols for primitives are in mixed case.
  // If you want to use lower or upper case, you can
  // specify that in the interpreter object.
  if (SymbolUpperCase == interp->symbolCase)
    StrToUpper(name);
  else if (SymbolLowerCase == interp->symbolCase)
    StrToLower(name);

  // Add name to symbol table.
  ListPush(interp->symbolTable, ItemWithString(name));
  
  // Add function to value table.
  Item item;
  item.type = TypePrimFun;
  item.symbol = interp->numberOfPrimFuns - 1;
  item.value.primFun = fun;
  ListPush(interp->gvarTable, item);

  // Free name.
  MemFree(name);
}

// VARIABLES ---------------------------------------------------

void InterpSetGlobal(Interp* interp, Item name, Item value)
{
  if (IsSymbol(name))
    ListSet(interp->gvarTable, name.symbol, value);
  else
    ErrorExit("InterpSetGlobal: Got a non-symbol");
}

#ifdef OPTIMIZE
#define InterpSetLocal(interp, name, item) \
  do { \
    Context* context = (interp)->currentContext; \
    ListAssocSet(context->env, (name).symbol, &(item)); \
  } while (0)
#else
void InterpSetLocal(Interp* interp, Item name, Item value)
{
  if (!IsSymbol(name))
    ErrorExit("InterpSetLocal: Got a non-symbol in function");

  // Get environment of current context.
  Context* context = interp->currentContext;

  // Error checking.
  if (!context)
    ErrorExit("InterpSetLocal: Context not found");
  if (!context->env)
    ErrorExit("InterpSetLocal: Context has no environment");

  // Set symbol value.
  //ListAssocSet(context->env, name.symbol, &value);
  ListAssocSet(context->env, name.symbol, &value);

  //PrintDebug("InterpSetLocal: PRINTING ENV");
  //ListPrint(context->env, interp);
}
#endif

Item InterpEvalSymbol(Interp* interp, Item item)
{
  // Look for symbol in current context.
  Context* context = interp->currentContext;
  if (context && context->env && (context->env->length > 0))
  {
    //PrintLine("InterpEvalSymbol: ENV");
    //ListPrint(context->env, interp);
    Item* value = ListAssocGet(context->env, item.symbol);
    if (value) return *value;
  }

  // Lookup global symbol.
  Item value = ListGet(interp->gvarTable, item.symbol);
  if (!IsVirgin(value)) return value;

  // Symbol is unbound and evaluates to itself.
  return item;
}

Item* POINTERBASED_InterpEvalSymbol(Interp* interp, Item* item)
{
  // Look for symbol in current context.
  Context* context = interp->currentContext;
  if (context && context->env && (context->env->length > 0))
  {
    //PrintLine("InterpEvalSymbol: ENV");
    //ListPrint(context->env, interp);
    Item* value = ListAssocGet(context->env, item->symbol);
    if (value) return value;
  }

  // Lookup global symbol.
  Item* value = ListItemPtr(interp->gvarTable, item->symbol);
  if (!IsVirgin(*value)) return value;

  // Symbol is unbound and evaluates to itself.
  return item;
}

// CALLSTACK CONTEXTS ------------------------------------------

#define InterpEnterContext(interp, code) \
  InterpEnterCallContext(interp, code, NULL, FALSE)

#define InterpEnterFunCallContext(interp, code) \
  InterpEnterCallContext(interp, code, NULL, TRUE)

#define InterpEnterContextWithEnv(interp, code, env) \
  InterpEnterCallContext(interp, code, env, FALSE)

void InterpEnterCallContext(Interp* interp, List* code, List* env, Bool isFunCall)
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
    //PrintDebug("TAILCALL AT INDEX: %i", interp->callstackIndex);
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
      nextContext = ContextCreate(interp);
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
  nextContext->code = code;
  nextContext->codePointer = -1;
  nextContext->isFunCall = isFunCall;

  // Set environment.
  if (isFunCall)
  {
#ifdef USE_GC
    // If env is shared (using BIND) then create a new one.
    if (nextContext->ownEnv->isShared)
      nextContext->ownEnv = InterpListCreate(interp);
#endif

    // Funcall contexts are initialized with an empty environment.
    nextContext->env = nextContext->ownEnv;
    ListEmpty(nextContext->env);
  }
  else if (NULL != env)
  {
    // Use supplied environment
    nextContext->env = env;
  }
  else
  {
    // Use environment of previous context.
    nextContext->env = currentContext->env;
  }

  // Set current context.
  interp->currentContext = nextContext;
}

// INTERPRETER LOOP --------------------------------------------

// FASTER (ORIGINAL VERSION USING ITEM COPYING)
// Evaluate list.
void InterpRun(register Interp* interp, List* list)
{
  register Context*   currentContext;
  register List*      code;
  register int        codePointer;
  register int        codeLength;
  register Item       element;
  register Item       evalResult;

  // Initialize interpreter state and create root context.
  interp->run = TRUE;
  interp->contextSwitch = TRUE;
  interp->callstackIndex = 0;
  interp->currentContext = interp->callstack;
  interp->currentContext->code = list;
  interp->currentContext->codePointer = -1;

  while (TRUE)
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

    // BEGIN EXIT STACKFRAME
    // Exit stackframe if the code in the current context 
    // has finished executing.
    if (codePointer >= codeLength)
    {
      PrintDebug("EXIT CONTEXT: %i", interp->callstackIndex);

      // Switch to parent context.
      currentContext = interp->currentContext = currentContext->prevContext;

      // Exit if this was the last stackframe
      if (!currentContext) goto Exit;
      
      code = currentContext->code;
      codeLength = ListLength(code);

#ifndef OPTIMIZE // ! OPTIMIZE
      -- interp->callstackIndex;
      if (interp->currentContext)
      {
        ContextFree(interp->currentContext->nextContext);
        interp->currentContext->nextContext = NULL;
      }
#endif
      goto Next; //continue;
    }
    // END EXIT STACKFRAME

    // Get next element.
    element = ListGet(code, codePointer);

#ifdef OPTIMIZE_PRIMFUNS
    // Optimized primfun calls
    if (IsPrimFun(element))
    {
      element.value.primFun(interp);
      goto Next; //continue;
    }
#endif

    if (IsSymbol(element))
    {
      // Evaluate symbol (search local and global env).
      // Symbols evaluate to themselves if unbound.
      evalResult = InterpEvalSymbol(interp, element);
      
#ifndef OPTIMIZE_PRIMFUNS // ! OPTIMIZE_PRIMFUNS     
      if (IsPrimFun(evalResult))
      {
        evalResult.value.primFun(interp);
        goto Next; //continue;
      }
#endif
      // If it is a function call it.
      if (IsFun(evalResult))
      {
        InterpEnterFunCallContext(interp, evalResult.value.list);

        currentContext = interp->currentContext;
        code = currentContext->code;
        codeLength = ListLength(code);
        interp->contextSwitch = FALSE;

        goto Next; //continue;
      }

      if (IsSpecialFun(evalResult))
      {
        InterpEnterContext(interp, evalResult.value.list);

        currentContext = interp->currentContext;
        code = currentContext->code;
        codeLength = ListLength(code);
        interp->contextSwitch = FALSE;

        goto Next; //continue;
      }

      // If not a function, push the symbol value.
      ListPush(interp->stack, evalResult);
      goto Next; //continue;
    }

    // If none of the above, push the element.
    ListPush(interp->stack, element);
Next:;
  } // while
Exit:;
}

// SLOWER (ITEM POINTERS)
// Evaluate list.
void POINTERBASED_InterpRun(register Interp* interp, List* list)
{
  register Context*   currentContext;
  register List*      code;
  register int        codePointer;
  register int        codeLength;
  register Item*      element;
  register Item*      evalResult;

  // Initialize interpreter state and create root context.
  interp->run = TRUE;
  interp->contextSwitch = TRUE;
  interp->callstackIndex = 0;
  interp->currentContext = interp->callstack;
  interp->currentContext->code = list;

  while (TRUE)
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

    // BEGIN EXIT STACKFRAME
    // Exit stackframe if the code in the current context 
    // has finished executing.
    if (codePointer >= codeLength)
    {
      PrintDebug("EXIT CONTEXT: %i", interp->callstackIndex);

      // TODO: Allow a "primfun callback" that is called on exit frame ???

      // Switch to parent context.
      currentContext = interp->currentContext = currentContext->prevContext;

      // Exit if this was the last stackframe
      if (!currentContext) goto Exit;
      
      code = currentContext->code;
      codeLength = ListLength(code);

#ifndef OPTIMIZE // ! OPTIMIZE
      -- interp->callstackIndex;
      if (interp->currentContext)
      {
        ContextFree(interp->currentContext->nextContext);
        interp->currentContext->nextContext = NULL;
      }
#endif
      goto Next; //continue;
    }
    // END EXIT STACKFRAME

    // Get next element.
    element = ListItemPtr(code, codePointer);

#ifdef OPTIMIZE_PRIMFUNS
    // Optimized primfun calls
    if (IsPrimFun(*element))
    {
      element->value.primFun(interp);
      goto Next; //continue;
    }
#endif

    if (IsSymbol(*element))
    {
      // Evaluate symbol (search local and global env).
      // Symbols evaluate to themselves if unbound.
      evalResult = POINTERBASED_InterpEvalSymbol(interp, element);
      
#ifndef OPTIMIZE_PRIMFUNS // ! OPTIMIZE_PRIMFUNS     
      if (IsPrimFun(*evalResult))
      {
        evalResult->value.primFun(interp);
        goto Next; //continue;
      }
#endif
      // If it is a function call it.
      if (IsFun(*evalResult))
      {
        InterpEnterFunCallContext(interp, evalResult->value.list);

        currentContext = interp->currentContext;
        code = currentContext->code;
        codeLength = ListLength(code);
        interp->contextSwitch = FALSE;

        goto Next; //continue;
      }

      if (IsSpecialFun(*evalResult))
      {
        InterpEnterContext(interp, evalResult->value.list);

        currentContext = interp->currentContext;
        code = currentContext->code;
        codeLength = ListLength(code);
        interp->contextSwitch = FALSE;

        goto Next; //continue;
      }

      // If not a function, push the symbol value.
      ListPush(interp->stack, *evalResult);
      goto Next; //continue;
    }

    // If none of the above, push the element.
    ListPush(interp->stack, *element);
Next:;
  } // while
Exit:;
}
