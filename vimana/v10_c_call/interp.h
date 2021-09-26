/*
File: interp.h
Author: Mikael Kindborg (mikael@kindborg.com)

Interpreter core.
*/

// DECLARATIONS ------------------------------------------------

// CONTEXT -----------------------------------------------------

// A context is a stackframe on the callstack.
// It is used to track code execution of code blocks and functions.
typedef struct __VContext
{
  VList* codeList;
  VIndex codePointer;
}
VContext;

#include "contextlist_gen.h"

// INTERPRETER -------------------------------------------------

typedef struct __VInterp
{
  VList*  globalVars;        // List of global variable values
  VList*  stack;             // The data stack
  VList*  callstack;         // Callstack with context frames
  VIndex  callstackIndex;    // Index of current frame
  VBool   run;               // Run flag
  VBool   contextSwitch;
  VGarbageCollector* gc;     // Garbage collector
  long    numContextCalls;   // Number of context pushes
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
#define InterpCallStackIndex(interp) \
  ((interp)->callstackIndex)
#define InterpContext(interp) \
  (ContextList_Get(InterpCallStack(interp), InterpCallStackIndex(interp)))
#define InterpCodeList(interp) \
  (InterpContext(interp)->codeList)
#define InterpCodePointer(interp) \
  (InterpContext(interp)->codePointer)
#define InterpCurrentCodeItem(interp) \
  (ItemList_Get(InterpCodeList(interp), InterpCodePointer(interp)))

// CREATE/FREE FUNCTIONS ---------------------------------------

VInterp* InterpCreate()
{
  VInterp* interp = MemAlloc(sizeof(VInterp));
  InterpGlobalVars(interp) = ItemList_Create();
  InterpStack(interp) = ItemList_Create();
  InterpCallStack(interp) = ContextList_Create();
  interp->gc = GCCreate();
  interp->numContextCalls = 0;
  return interp;
}

void InterpFree(VInterp* interp)
{
#ifdef DEBUG
  Print("STACK:");
  PrintList(InterpStack(interp));
  PrintNewLine();
  Print("GLOBALS:");
  PrintList(InterpGlobalVars(interp));
  PrintNewLine();
  PrintLine("CONTEXT CALLS:");
  PrintNum(interp->numContextCalls);
  PrintNewLine();
#endif

  // Free non-gc managed objects.
  ListFree(InterpGlobalVars(interp));
  ListFree(InterpStack(interp));
  ListFree(InterpCallStack(interp));
  GCFree(interp->gc);

  // Free interpreter struct.
  MemFree(interp);
}

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

// Push root context with codeList.
void InterpInit(VInterp* interp, VList* codeList)
{
  ListLength(InterpCallStack(interp)) = 0;
  ListPushRaw(InterpCallStack(interp));
  InterpCallStackIndex(interp) = 0;
  InterpCodeList(interp) = codeList;
  InterpCodePointer(interp) = -1;
  //PrintDebugStrNum("ENTER ROOT CONTEXT: ", InterpCallStackIndex(interp));
}

void InterpPushContext(VInterp* interp, VList* codeList)
{
  ++ interp->numContextCalls;

  interp->contextSwitch = TRUE;

  VBool isTailCall = 
    (1 + InterpCodePointer(interp)) >= 
    ListLength(InterpCodeList(interp));

  if (isTailCall)
  {
    //PrintDebugStrNum("TAILCALL CONTEXT: ", InterpCallStackIndex(interp));
  }
  else
  {
    ++ InterpCallStackIndex(interp);
    if (InterpCallStackIndex(interp) >= ListLength(InterpCallStack(interp)))
      ListPushRaw(InterpCallStack(interp));
    //PrintDebugStrNum("ENTER CONTEXT: ", InterpCallStackIndex(interp));
  }

  InterpCodeList(interp) = codeList;
  InterpCodePointer(interp) = -1;
}

#define InterpPopContext(interp) \
  (-- InterpCallStackIndex(interp))

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
  register VItem*  item;
  register VNumber sliceCounter = 0;
  register VIndex  codePointer;
  register VList*  code;
  register VSize   codeLength;

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
      code = InterpCodeList(interp);
      codeLength = ListLength(code);
    }

    // Increment code pointer.
    codePointer = ++ InterpCodePointer(interp);
    
    //PrintDebugStrNum("CODE POINTER: ", InterpCodePointer(interp));

    // Call item function if not end of list. 
    if (codePointer < codeLength)
    {
      // Call current item in the code list.
      //item = InterpCurrentCodeItem(interp);
      item = ItemList_Get(code, codePointer);
      ItemFun(item)(interp, item);
    }
    else
    // Exit stackframe.
    {
      //PrintDebugStrNum("EXIT CONTEXT: ", InterpCallStackIndex(interp));

      // Pop stackframe.
      InterpPopContext(interp);
      interp->contextSwitch = TRUE;

      // Exit interpreter loop if this was the last stackframe.
      if (InterpCallStackIndex(interp) < 0) 
        interp->run = FALSE;
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
      goto Exit;
    }

    // If it is a function, call it.
    if (IsFun(symbolValue))
    {
      InterpPushContext(interp, ItemList(symbolValue));
      goto Exit;
    }

    //PrintDebug("PUSH SYMBOL VALUE");
    // If not a function, push the symbol value.
    InterpPush(interp, symbolValue);
    goto Exit;
  }
Exit:;
}

VFunPtr GInterpNumberFun = InterpNumberFun;
VFunPtr GInterpListFun   = InterpListFun;
VFunPtr GInterpStringFun = InterpStringFun;
VFunPtr GInterpSymbolFun = InterpSymbolFun;
