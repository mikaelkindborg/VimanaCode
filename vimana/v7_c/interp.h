
// Context environment handling.
#define ContextResetEnv     0
#define ContextBorrowEnv    1

// Declarations.
Item EvalCoreEvalSymbol(Interp* interp, Item item);
void EvalCoreEvalFun(Interp* interp, List* fun);

// Octo: That was new

// With freedom comes responsibilities (for the programmer).
// Dynamic languages in action.

/***** INTERPRETER ***************************************/

typedef struct MyInterp
{
  List* globalSymbolTable; // List of symbols
  List* globalValueTable;  // List of global values (primfuns, funs, etc)
  List* stack;             // The data stack
  List* callstack;         // Callstack with context frames
  int   callstackIndex;    // Index of current frame
  Bool  run;               // Run flag
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
  interp->run = TRUE;
  return interp;
}

void InterpFree(Interp* interp)
{
  ListFree(interp->globalSymbolTable, ListFreeDeep);
  ListFree(interp->globalValueTable, ListFreeShallow);
  ListFree(interp->stack, ListFreeShallow);
  ListFree(interp->callstack, ListFreeDeep);
}

/***** CONTEXT *******************************************/

typedef struct MyContext
{
  List* env;
  List* code;
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
  ListFree(context->env, ListFreeShallow);
  free(context);
}

#define ContextEnv(context) ((context)->env)

/***** DATA STACK ****************************************/

// Push an item onto the data stack.
#define InterpPush(interp, item) ListPush((interp)->stack, item)

// Pop an item from the data stack and set variable to it.
#define InterpPopSet(interp, item) ListPopSet((interp)->stack, item)

// Pop and evaluate an item from the stack and set variable to it.
#define InterpPopEvalSet(interp, item) \
  do { \
    ListPopSet((interp)->stack, item); \
    (item) = IsSymbol(item) ? \
      EvalCoreEvalSymbol(interp, item) : \
      item; \
  } while(0)

/*
#define InterpPopEvalSet(interp, item) \
  do { \
    ListPopSet((interp)->stack, item); \
    (item) = (IsSymbol(item) || IsLocalVar(item)) ? \
      EvalCoreEvalSymbol(interp, item) : \
      item; \
  } while(0)
*/
/***** SYMBOL TABLE **************************************/

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

void InterpAddPrimFun(char* name, PrimFun fun, Interp* interp)
{
  // Add name to symbol table.
  ListPush(interp->globalSymbolTable, ItemWithString(name));
  
  // Add function to value table.
  ListPush(interp->globalValueTable, ItemWithPrimFun(fun));
}

/***** CALL STACK ****************************************/

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
  ++ callstackIndex;
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
  --interp->callstackIndex;
#ifndef OPTIMIZE
  Context* context = ItemContext(ListPop(interp->callstack));
  ContextFree(context);
#endif
}

/**** EVAL LIST ******************************************/

void InterpEvalList(Interp* interp, List* code)
{
  InterpEnterContext(interp, code, ContextBorrowEnv);
}

/**** MAIN INTERPRETER LOOP ******************************/

void InterpRun(Interp* interp, List* list)
{
  // Create root context.
  InterpEvalList(interp, list);

  while (interp->run)
  { 
    // Get current context.
    Context* context = ItemContext(ListGet(interp->callstack, interp->callstackIndex));

    // Increment code pointer.
    ++ context->codePointer;

    // Exit the frame if the code in the current context has finished executing.
    List* code = context->code;
    if (context->codePointer >= ListLength(code))
    {
      InterpExitContext(interp);
      //-- interp->callstackIndex;
      goto exit;
    }

    // Get the next element.
    Item element = ListGet(code, context->codePointer);

#ifdef OPTIMIZE
    // If the elements points directly to a primfun we run it.
    if (IsPrimFun(element))
    {
      PrintDebug("CALLING PRIMFUN");
      element.value.primFun(interp);
      goto exit;
    }
#endif

    // If symbol we check the global value is primfun or fun.
    // This means that functions defined with DEF will take 
    // precedence over local variables with the same name. 
    // Furthermore, local functions need to be called with CALL.
    if (IsSymbol(element))
    {
      Item value = ListGet(interp->globalValueTable, element.value.symbol);
      if (IsPrimFun(value))
      {
        value.value.primFun(interp);
        goto exit;
      }
      if (IsFun(value))
      {
        EvalCoreEvalFun(interp, value.value.list);
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
  }
}
