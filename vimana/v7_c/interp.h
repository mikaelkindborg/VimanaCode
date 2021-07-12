
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
  Bool  releaseEnv;  // Release ownEnv on stackframe exit
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
// There are speed benefits to this setup and dirent lookup
// by index can be used for global vars. This scheme is intended
// to be a simplistic and straightforward solution.

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

Context* ContextCreate(Interp* interp)
{
  Context* context = malloc(sizeof(Context));
#ifdef USE_GC
  // Environments may be shared by closures,
  // so we allocate it using the GC.
  context->ownEnv = GCListCreate(interp->gc);
#else
  context->ownEnv = ListCreate();
#endif
  context->env = context->ownEnv;
  context->releaseEnv = FALSE;
  context->code = NULL;
  context->codePointer = -1;
  context->nextContext = NULL;
  context->prevContext = NULL;
  return context;
}

void ContextFree(Context* context)
{
  PrintDebug("ContextFree ownEnv: %lu", (unsigned long)(context->ownEnv));
#ifndef USE_GC
  ListFree(context->ownEnv);
#endif
  free(context);
}

Interp* InterpCreate()
{
  Interp* interp = malloc(sizeof(Interp));
  interp->symbolTable = ListCreate();
  interp->gvarTable = ListCreate();
  interp->stack = ListCreate();
  interp->symbolCase = SymbolUpperCase; // Default
  interp->numberOfPrimFuns = 0;
#ifdef USE_GC
  interp->gc = GCCreate();
#endif
  return interp;
}

// TODO: Improve deallocation.
void InterpFree(Interp* interp)
{
  // TODO: Free strings in symbolTable.
  ListFree(interp->symbolTable);
  ListFree(interp->gvarTable);
  ListFree(interp->stack);
  // TODO: Free interp->callstack
#ifdef USE_GC
  GCFree(interp->gc);
#endif
  free(interp);
}

// GARBAGE COLLECTION ------------------------------------------

#ifdef USE_GC
void InterpGC(Interp* interp)
{
  GCPrintEntries(interp->gc);

  GCPrepare();

  // Mark items on the stack.
  GCMarkChildren(interp->stack, 0);

  // Mark global variables (skipping primfun entries).
  GCMarkChildren(interp->gvarTable, interp->numberOfPrimFuns);

  // Mark objects in local callstack environments.
  Context* context = interp->callstack;
  while (context)
  {
    GCMarkList(context->ownEnv);
    context = context->nextContext;
  }

  // Free unreachable objects.
  GCSweep(interp->gc);

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
  return item.value.string;
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
  ListPush(interp->symbolTable, ItemWithString(name));
  
  // Add function to value table.
  Item item;
  item.type = TypePrimFun;
  item.opCode = OpCodeCallPrimFun;
  item.value.primFun = fun;
  ListPush(interp->gvarTable, item);

  // Free name.
  free(name);
}

// VARIABLES ---------------------------------------------------

void InterpSetGlobal(Interp* interp, Item name, Item value)
{
  if (IsSymbol(name))
    ListSet(interp->gvarTable, name.value.symbol, value);
  else
    ErrorExit("InterpSetGlobal: Got a non-symbol");
}

#ifdef OPTIMIZE
#define InterpSetLocal(interp, name, item) \
  do { \
    if (!IsSymbol(name)) \
      ErrorExit("InterpSetLocal: Got a non-symbol in macro"); \
    Context* context = (interp)->currentContext; \
    ListAssocSet(context->env, (name).value.symbol, &(item)); \
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
  //ListAssocSet(context->env, name.value.symbol, &value);
  ListAssocSet(context->env, name.value.symbol, &value);

  //PrintDebug("InterpSetLocal: PRINTING ENV");
  //ListPrint(context->env, interp);
}
#endif

Item InterpEvalSymbol(Interp* interp, Item item)
{
  // Non-symbols evaluates to themselves.
  if (!IsSymbol(item))
    return item;

  // Look for symbol in current context.
  Context* context = interp->currentContext;
  if (context && context->env)
  {
    //PrintDebug("InterpEvalSymbol: ENV");
    //ListPrint(context->env, interp);
    Item* value = ListAssocGet(context->env, item.value.symbol);
    if (value)
      return *value;
  }

  // Lookup global symbol.
  Item value = ListGet(interp->gvarTable, item.value.symbol);
  if (!IsVirgin(value)) 
    return value;

  // Symbol is unbound and evaluates to itself.
  return item;
}

// CALLSTACK ---------------------------------------------------

#define InterpEnterContext(interp, code) \
  InterpEnterCallContext(interp, code, FALSE)

#define InterpEnterFunCallContext(interp, code) \
  InterpEnterCallContext(interp, code, TRUE)

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
/*
  // Should we release the context env (flag set when used by closure).
  if (nextContext->releaseEnv)
  {
    // Closure uses the environment, create new env for context.
#ifdef USE_GC
    nextContext->ownEnv = GCListCreate(interp->gc);
#else
    // Causes memory leak, you should use GC with closures.
    nextContext->ownEnv = ListCreate();
#endif
    nextContext->releaseEnv = FALSE;
  }
*/
  // Set context data.
  nextContext->code = code;
  nextContext->codePointer = -1;

  // Set environment.
  if (isFunCall)
  {
    // Use fresh (empty) environment.
    nextContext->env = nextContext->ownEnv;
    ListEmpty(nextContext->env);
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

// INTERPRETER LOOP --------------------------------------------

//#define USE_ORIGINAL_LOOP_WITH_SOME_GOTOS
#define USE_NO_GOTOS_IN_LOOP
//#define USE_COMPUTED_GOTOS

#ifdef USE_ORIGINAL_LOOP_WITH_SOME_GOTOS
// Evaluate list.
void InterpRun(Interp* interp, List* list)
{
  Context*   currentContext;
  List*      code;
  int        codePointer;
  int        codeLength;
  Item       element;
  Item       item;

  // Initialize interpreter state and create root context.
  interp->run = TRUE;
  interp->contextSwitch = TRUE;
  interp->callstackIndex = 0;
  interp->callstack = ContextCreate(interp);
  interp->currentContext = interp->callstack;
  interp->currentContext->code = list;

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

DoNextInstr:
    // Increment code pointer.
    codePointer = ++ currentContext->codePointer;

    // BEGIN EXIT STACKFRAME
    // Exit stackframe if the code in the current context 
    // has finished executing.
    if (codePointer >= codeLength)
    {
      PrintDebug("EXIT CONTEXT: %i", interp->callstackIndex);
      interp->contextSwitch = TRUE;

      // Release the context env (flag set when used by closure).
      if (currentContext->releaseEnv)
      {
        // Closure uses the environment, create new env for context.
#ifdef USE_GC
        currentContext->ownEnv = GCListCreate(interp->gc);
#else
        // Causes memory leak, you should use GC with closures.
        currentContext->ownEnv = ListCreate();
#endif
        currentContext->releaseEnv = FALSE;
      }

      // Switch to parent context.
      interp->currentContext = currentContext->prevContext;

#ifndef OPTIMIZE // ! OPTIMIZE
      -- interp->callstackIndex;
      if (interp->currentContext)
      {
        ContextFree(interp->currentContext->nextContext);
        interp->currentContext->nextContext = NULL;
      }
#endif
      goto DoExit;
    }
    // END EXIT STACKFRAME

    // Get next element.
    element = ListGet(code, codePointer);

#ifdef OPTIMIZE_PRIMFUNS
    // Optimized primfun calls
    if (IsPrimFun(element))
    {
      element.value.primFun(interp);
      goto DoExit;
    }
#endif

    if (IsSymbol(element))
    {
      // Evaluate symbol (search local and global env).
      // Symbols evaluate to themselves if unbound.
      item = InterpEvalSymbol(interp, element);
      
#ifndef OPTIMIZE_PRIMFUNS // ! OPTIMIZE_PRIMFUNS     
      if (IsPrimFun(item))
      {
        item.value.primFun(interp);
        goto DoExit;
      }
#endif
      // If it is a function call it.
      if (IsFun(item))
      {
        InterpEnterFunCallContext(interp, item.value.list);
        goto DoExit;
      }

      // If not a function, push the symbol value.
      ListPush(interp->stack, item);
      //goto DoNextInstr; // Slows down a lot!
      goto DoExit;
    }

    // If none of the above, push the element.
    ListPush(interp->stack, element);
    //goto DoNextInstr; // Slows down somewhat.

DoExit:
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
#endif

// INTERPRETER LOOP WITH NO GOTOS ------------------------------

#ifdef USE_NO_GOTOS_IN_LOOP 
// Evaluate list.
void InterpRun(Interp* interp, List* list)
{
  Context*   currentContext;
  List*      code;
  int        codePointer;
  int        codeLength;
  Item       element;
  Item       item;

  // Initialize interpreter state and create root context.
  interp->run = TRUE;
  interp->contextSwitch = TRUE;
  interp->callstackIndex = 0;
  interp->callstack = ContextCreate(interp);
  interp->currentContext = interp->callstack;
  interp->currentContext->code = list;

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

    // BEGIN EXIT STACKFRAME
    // Exit stackframe if the code in the current context 
    // has finished executing.
    if (codePointer < codeLength)
    {
      // Get next element.
      element = ListGet(code, codePointer);
  #ifdef OPTIMIZE_PRIMFUNS
      // Optimized primfun calls
      if (IsPrimFun(element))
      {
        element.value.primFun(interp);
      }
      else
  #endif
      if (IsSymbol(element))
      {
        // Evaluate symbol (search local and global env).
        // Symbols evaluate to themselves if unbound.
        item = InterpEvalSymbol(interp, element);
  #ifndef OPTIMIZE_PRIMFUNS // ! OPTIMIZE_PRIMFUNS     
        if (IsPrimFun(item))
        {
          item.value.primFun(interp);
        }
        else
  #endif
        // If it is a function call it.
        if (IsFun(item))
        {
          InterpEnterFunCallContext(interp, item.value.list);
        }
        else
        {
          // If not a function, push the symbol value.
          ListPush(interp->stack, item);
        }
      }
      else
      {
        // If none of the above, push the element.
        ListPush(interp->stack, element);
      }
    }
    else
    {
      PrintDebug("EXIT CONTEXT: %i", interp->callstackIndex);
      interp->contextSwitch = TRUE;
/*
      // Release the context env (flag set when used by closure).
      if (currentContext->releaseEnv)
      {
        // Closure uses the environment, create new env for context.
        currentContext->ownEnv = ListCreate();
        currentContext->releaseEnv = FALSE;
      }
*/
      // Switch to parent context.
      interp->currentContext = currentContext->prevContext;

#ifndef OPTIMIZE // ! OPTIMIZE
      -- interp->callstackIndex;
      if (interp->currentContext)
      {
        ContextFree(interp->currentContext->nextContext);
        interp->currentContext->nextContext = NULL;
      }
#endif
    }
    // END EXIT STACKFRAME

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
#endif

// INTERPRETER LOOP WITH COMPUTED GOTOS ------------------------

#ifdef USE_COMPUTED_GOTOS
// Check this out: https://eli.thegreenplace.net/2012/07/12/computed-goto-for-efficient-dispatch-tables/
// Evaluate list.
void InterpRun(Interp* interp, List* list)
{
  Context*   currentContext;
  List*      code;
  int        codePointer;
  int        codeLength;
  Item       element;
  Item       item;

  static void* dispatchTable[] = 
  { 
    &&DoExit // OpCodeNone
    , &&DoPushItem
    , &&DoEvalSymbol
#ifdef OPTIMIZE_PRIMFUNS
    , &&DoCallPrimFun
#endif
  };

  // Initialize interpreter state and create root context.
  interp->run = TRUE;
  interp->contextSwitch = TRUE;
  interp->callstackIndex = 0;
  interp->callstack = ContextCreate(interp);
  interp->currentContext = interp->callstack;
  interp->currentContext->code = list;

DoLoop:
    // Check context switch.
    if (interp->contextSwitch)
    {
      interp->contextSwitch = FALSE;
      currentContext = interp->currentContext;
      code = currentContext->code;
      codeLength = ListLength(code);
    }

DoNextInstr:
    // Increment code pointer.
    codePointer = ++ currentContext->codePointer;

    // BEGIN EXIT STACKFRAME
    // Exit stackframe if the code in the current context 
    // has finished executing.
    if (codePointer >= codeLength)
    {
      PrintDebug("EXIT CONTEXT: %i", interp->callstackIndex);
      interp->contextSwitch = TRUE;
/*
      // Release the context env (flag set when used by closure).
      if (currentContext->releaseEnv)
      {
        // Closure uses the environment, create new env for context.
        //currentContext->ownEnv = ListCreate();
        //currentContext->releaseEnv = FALSE;
      }
*/
      // Switch to parent context.
      interp->currentContext = currentContext->prevContext;

#ifndef OPTIMIZE // ! OPTIMIZE
      -- interp->callstackIndex;
      if (interp->currentContext)
      {
        ContextFree(interp->currentContext->nextContext);
        interp->currentContext->nextContext = NULL;
      }
#endif
      goto DoExit;
    }
    // END EXIT STACKFRAME

    // Get next element.
    element = ListGet(code, codePointer);

    // Dispatch on item opcode.
    //PrintDebug("DISPATCH OPCODE: %d", element.opCode);
    goto *dispatchTable[element.opCode];

#ifdef OPTIMIZE_PRIMFUNS
DoCallPrimFun:
    // Optimized primfun calls
    element.value.primFun(interp);
    goto DoLoop;
#endif

DoEvalSymbol:
    // Evaluate symbol (search local and global env).
    // Symbols evaluate to themselves if unbound.
    item = InterpEvalSymbol(interp, element);
      
#ifndef OPTIMIZE_PRIMFUNS // ! OPTIMIZE_PRIMFUNS     
    if (IsPrimFun(item))
    {
      item.value.primFun(interp);
      goto DoLoop;
    }
#endif
    // If it is a function call it.
    if (IsFun(item))
    {
      InterpEnterFunCallContext(interp, item.value.list);
      goto DoLoop;
    }

    // If not a function, push the symbol value.
    ListPush(interp->stack, item);
    goto DoLoop;

DoPushItem:
    // If none of the above, push the element.
    ListPush(interp->stack, element);
    goto DoNextInstr;

DoExit:
    // Do we have a stackframe?
    if (NULL != interp->currentContext)
    {
      goto DoLoop;
    }

    PrintDebug("EXIT InterpRun callstackIndex: %i", interp->callstackIndex);
}
#endif
