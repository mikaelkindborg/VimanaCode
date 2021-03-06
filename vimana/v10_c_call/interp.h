/*
File: interp.h
Author: Mikael Kindborg (mikael@kindborg.com)

Interpreter core.
*/

// CONTEXT -----------------------------------------------------

// A context is a stackframe on the callstack.
// It is used to track code execution of code blocks and functions.
typedef struct __VContext
{
  VList* codeList;
  VIndex codePointer;
  struct __VContext* next;
  struct __VContext* prev;
}
VContext;

// CONTEXT CREATE/FREE -----------------------------------------

VContext* ContextCreate()
{
  VContext* context = MemAlloc(sizeof(VContext));
  context->codeList = NULL;
  context->codePointer = -1;
  context->next = NULL;
  context->prev = NULL;
  return context;
}

// Free self and all children.
void ContextFree(VContext* context)
{
  VContext* next = context->next;
  if (next)
  {
    ContextFree(next);
  }
  MemFree(context);
}

// INTERPRETER -------------------------------------------------

typedef struct __VInterp
{
  VList*     globalVars;        // List of global variable values
  VList*     stack;             // The data stack
  VContext*  callstack;         // Callstack context
  VContext*  currentContext;
  VBool      run;               // Run flag
  VBool      contextSwitch;
  VGarbageCollector* gc;        // Garbage collector
  long       numContextCalls;   // Number of context pushes
  //long    wakeUpTime;        // Time to wake up after sleep
}
VInterp;

// ACCESSORS ---------------------------------------------------

#define InterpGlobalVars(interp) \
  ((interp)->globalVars)
#define InterpStack(interp) \
  ((interp)->stack)
#define InterpCallStack(interp) \
  ((interp)->callstack)
#define InterpCurrentContext(interp) \
  ((interp)->currentContext)

// CREATE/FREE FUNCTIONS ---------------------------------------

VInterp* InterpCreate()
{
  VInterp* interp = MemAlloc(sizeof(VInterp));
  InterpGlobalVars(interp) = ItemList_Create();
  InterpStack(interp) = ItemList_Create();
  InterpCallStack(interp) = ContextCreate();
  interp->gc = GCCreate();
  interp->numContextCalls = 0;
  return interp;
}

void InterpFree(VInterp* interp)
{
#ifdef DEBUG
  PrintLine("DEALLOCATING INTERPETER:");
  Print("STACK:");
  PrintList(InterpStack(interp));
  PrintNewLine();
  Print("GLOBALS:");
  PrintList(InterpGlobalVars(interp));
  PrintNewLine();
  PrintStrNum("CONTEXT CALLS: ", interp->numContextCalls);
#endif

  // Free non-gc managed objects.
  ListFree(InterpGlobalVars(interp));
  ListFree(InterpStack(interp));
  ContextFree(InterpCallStack(interp));
  GCFree(interp->gc);

  // Free interpreter struct.
  MemFree(interp);
}

// Run garbage collector.
void InterpGC(VInterp* interp)
{
  GCMarkChildren(InterpGlobalVars(interp));
  GCMarkChildren(InterpStack(interp));
  GCSweep(interp->gc);
}

// GLOBAL VARIABLES --------------------------------------------

VItem* InterpGetGlobalVar(VInterp* interp, VIndex index)
{
  if (index < ListLength(InterpGlobalVars(interp)))
  {
    VItem* item = ItemList_Get(InterpGlobalVars(interp), index);
    if (!IsVirgin(item)) 
      return item;
  }
  
  // No value found
  return NULL;
}

#define InterpSetGlobalVar(interp, index, item) \
  ItemList_Set(InterpGlobalVars(interp), index, item)

// DATA STACK --------------------------------------------------

// Push an item onto the data stack.
#define InterpPush(interp, item) ItemList_Push(InterpStack(interp), item)

// Pop an item off the data stack.
#define InterpPop(interp) ItemList_Pop(InterpStack(interp))

void InterpClearStack(VInterp* interp)
{
  ListLength(InterpStack(interp)) = 0;
}

// CONTEXT HANDLING --------------------------------------------

void InterpInit(VInterp* interp, VList* codeList)
{
  VContext* context = InterpCallStack(interp);
  InterpCurrentContext(interp) = context;
  context->codeList = codeList;
  context->codePointer = -1;
}

void InterpPushContext(VInterp* interp, VList* codeList)
{
#ifdef DEBUG
  ++ interp->numContextCalls;
#endif

  interp->contextSwitch = TRUE;

  VContext* currentContext = InterpCurrentContext(interp);

  VBool isTailCall = 
    (1 + currentContext->codePointer) >= 
    ListLength(currentContext->codeList);

  if (!isTailCall)
  {
    if (!currentContext->next)
    {
      currentContext->next = ContextCreate();
      currentContext->next->prev = currentContext;
    }
    
    currentContext = currentContext->next;
    InterpCurrentContext(interp) = currentContext;
  }

  currentContext->codeList = codeList;
  currentContext->codePointer = -1;
}

// INTERPRETER LOOP --------------------------------------------

VBool InterpEvalSlice(register VInterp* interp, VNumber sliceSize);

void InterpEval(VInterp* interp, VList* codeList)
{
  GCAddObj(interp->gc, AsObj(codeList));

  InterpInit(interp, codeList);
  InterpEvalSlice(interp, 0);

  InterpGC(interp);
}

// Test InterpEvalSlice.
void InterpEvalTest(VInterp* interp, VList* codeList)
{
  GCAddObj(interp->gc, AsObj(codeList));

  InterpInit(interp, codeList);
  while ( ! InterpEvalSlice(interp, 100) ) { };

  InterpGC(interp);
}

// Evaluate a slice of code. 
// sliceSize specifies the number of instructions to execute.
// sliceSize 0 means eval as one slice until program ends.
// Returns done flag (TRUE = done, FALSE = not done).
VBool InterpEvalSlice(register VInterp* interp, register VNumber sliceSize)
{
  register VItem*    item;
  register VContext* currentContext;
  register VIndex    codePointer;
  register VList*    codeList;
  register VSize     codeLength;
  register VNumber   sliceCounter = 0;

  interp->run = TRUE;
  interp->contextSwitch = TRUE;

  while (interp->run)
  {
    // Track slices if a sliceSize is specified.
    if (sliceSize)
    {
      if (sliceSize > sliceCounter)
        ++ sliceCounter;
      else
        goto Exit;
    }

    if (interp->contextSwitch)
    {
      interp->contextSwitch = FALSE;
      currentContext = InterpCurrentContext(interp);
      codeList = currentContext->codeList;
      codeLength = ListLength(codeList);
    }

    // Increment code pointer.
    codePointer = ++ currentContext->codePointer;
    
    // Call item function if not end of list.
    if (codePointer < codeLength)
    {
      item = ItemList_Get(codeList, codePointer);
      ItemFun(item)(interp, item);
    }
    else
    // Otherwise exit stackframe.
    {
      currentContext = currentContext->prev;
      if (currentContext)
      {
        InterpCurrentContext(interp) = currentContext;
        codeList = currentContext->codeList;
        codeLength = ListLength(codeList);
      }
      else
      {
        // Exit interpreter loop if this was the last stackframe.
        interp->run = FALSE;
        goto Exit;
      }
    }
  } 
  // while
  
Exit:
  return ! interp->run;
}

// DISPATCH FUNCTIONS ------------------------------------------

void InterpNumberFun(VInterp* interp, VItem* item)
{
  InterpPush(interp, item);
}

void InterpListFun(VInterp* interp, VItem* item)
{
  InterpPush(interp, item);
}

void InterpStringFun(VInterp* interp, VItem* item)
{
  InterpPush(interp, item);
}

void InterpSymbolFun(VInterp* interp, VItem* item)
{
  if (IsSymbol(item))
  {
    // Find symbol value in global env.
    // Symbols evaluate to themselves if unbound.
    VIndex index = ItemSymbol(item);
    VItem* symbolValue = InterpGetGlobalVar(interp, index);
    if (NULL == symbolValue)
    {
      // Symbol is unbound, push the symbol itself.
      InterpPush(interp, item);
    }
    else
    if (IsFun(symbolValue))
    {
      // If it is a function, call it.
      InterpPushContext(interp, ItemList(symbolValue));
    }
    else
    {
      // If not a function, push the symbol value.
      InterpPush(interp, symbolValue);
    }
  }
}

VFunPtr GInterpNumberFun = InterpNumberFun;
VFunPtr GInterpListFun   = InterpListFun;
VFunPtr GInterpStringFun = InterpStringFun;
VFunPtr GInterpSymbolFun = InterpSymbolFun;
