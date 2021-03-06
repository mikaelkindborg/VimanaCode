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
#ifdef PLATFORM_MINIMAL
  VByte  padding;
#endif
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
#define InterpCurrentContext(interp) \
  (ContextList_Get(InterpCallStack(interp), InterpCallStackIndex(interp)))
#define InterpCodeList(interp) \
  (InterpCurrentContext(interp)->codeList)
#define InterpCodePointer(interp) \
  (InterpCurrentContext(interp)->codePointer)
#define InterpCurrentElement(interp) \
  (ItemList_Get(InterpCodeList(interp), InterpCodePointer(interp)))

// CREATE/FREE FUNCTIONS ---------------------------------------

VInterp* InterpCreate()
{
  VInterp* interp = MemAlloc(sizeof(VInterp));
  InterpGlobalVars(interp) = ItemList_Create();
  InterpStack(interp) = ItemList_Create();
  InterpCallStack(interp) = ContextList_Create();
  return interp;
}

void InterpFree(VInterp* interp)
{
#ifdef DEBUG
  PrintLine("DEALLOCATING INTERPETER:");
  PrintLine("STACK:");
  PrintList(InterpStack(interp));
  PrintNewLine();
  Print("GLOBALS:");
  PrintList(InterpGlobalVars(interp));
  PrintNewLine();
#endif

  // Free globals, data stack and callstack.
  // Data stack and callstack are not handled by the garbage collector.
  ListGC(InterpGlobalVars(interp));
  ListFree(InterpStack(interp));
  ListFree(InterpCallStack(interp));

  // Free interpreter struct.
  MemFree(interp);
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

void InterpSetGlobalVar(VInterp* interp, VIndex index, VItem* newItem)
{
  VItem* oldItem = InterpGetGlobalVar(interp, index);
  if (NULL != oldItem) 
    ItemGC(oldItem);
  ItemIncrRefCount(newItem);
  ItemList_Set(InterpGlobalVars(interp), index, newItem);
}

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
// Free code list with: ListGC(codeList)
void InterpInit(VInterp* interp, VList* codeList)
{
  ListLength(InterpCallStack(interp)) = 0;
  ListPushRaw(InterpCallStack(interp));
  InterpCallStackIndex(interp) = 0;
  InterpCodeList(interp) = codeList;
  InterpCodePointer(interp) = -1;
}

void InterpPushContext(VInterp* interp, VList* codeList)
{
  VBool isTailCall = 
    (1 + InterpCodePointer(interp)) >= 
    ListLength(InterpCodeList(interp));
  if (!isTailCall)
  {
    ++ InterpCallStackIndex(interp);
    if (InterpCallStackIndex(interp) >= ListLength(InterpCallStack(interp)))
    {
      ListPushRaw(InterpCallStack(interp));
    }
  }

  InterpCodeList(interp) = codeList;
  InterpCodePointer(interp) = -1;
}

#define InterpPopContext(interp) \
  (-- InterpCallStackIndex(interp))

// INTERPRETER LOOP --------------------------------------------

VBool InterpEvalSlice(register VInterp* interp, VNumber sliceSize);

void InterpCallFun(VInterp* interp, VIndex funIndex)
{
  //0(loop)setglobal
  //(loop) ('HiWorld' print 2000 delay) def
  VItem* item = InterpGetGlobalVar(interp, funIndex);
  if (!IsFun(item))
  {
    // TODO: Meditate or wait and try again.
    PrintLine("You have lost badly");
    exit(0);
  }

  VList* codeList = ItemList(item);
  InterpInit(interp, codeList);
  InterpEvalSlice(interp, 0);
}

void InterpEval(VInterp* interp, VList* codeList)
{
  InterpInit(interp, codeList);
  InterpEvalSlice(interp, 0);
  ListGC(codeList);
  // TODO: Meditate if stack contains object references 
  // (the data stack is not reference counted)
}

void InterpEvalTest(VInterp* interp, VList* codeList)
{
  InterpInit(interp, codeList);
  while ( ! InterpEvalSlice(interp, 100) ) { };
  ListGC(codeList);
}

// Evaluate a slice of code. 
// sliceSize specifies the number of instructions to execute.
// sliceSize 0 means eval as one slice until program ends.
// Returns done flag (TRUE = done, FALSE = not done).
VBool InterpEvalSlice(register VInterp* interp, register VNumber sliceSize)
{
  register VItem*  element;
  register VItem*  symbolValue;
  register VIndex  primFun;
  register VNumber sliceCounter = 0;

  interp->run = TRUE;

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

    // Increment code pointer.
    ++ InterpCodePointer(interp);
    
    // Exit stackframe if the current context has finished executing.
    if (InterpCodePointer(interp) >= ListLength(InterpCodeList(interp)))
    {
      // Pop stackframe.
      InterpPopContext(interp);

      // Exit run loop if this was the last stackframe.
      if (InterpCallStackIndex(interp) < 0)
      {
        interp->run = FALSE;
      }

      goto Next;
    }

    // Get current element in the code list.
    element = InterpCurrentElement(interp);

    if (IsPrimFun(element))
    {
      primFun = ItemPrimFun(element);
      #include "primfuns_gen.h"
      goto Next;
    }

    if (IsSymbol(element))
    {
      // Find symbol value in global env.
      // Symbols evaluate to themselves if unbound.
      VIndex index = ItemSymbol(element);
      symbolValue = InterpGetGlobalVar(interp, index);
      if (NULL == symbolValue)
      {
        // Symbol is unbound, push the symbol itself.
        InterpPush(interp, element);
        goto Next;
      }

      // If it is a function, call it.
      if (IsFun(symbolValue))
      {
        InterpPushContext(interp, ItemList(symbolValue));
        goto Next;
      }

      // If not a function, push the symbol value.
      InterpPush(interp, symbolValue);
      goto Next;
    }

    // If none of the above, push the element.
    InterpPush(interp, element);

Next:;
  } // while

Exit:
  return ! interp->run;
}
