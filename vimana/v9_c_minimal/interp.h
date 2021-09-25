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
  #ifdef PLATFORM_ARDUINO
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
  long    wakeUpTime;        // Time to wake up after sleep
  long    numContextCalls;
#ifdef GC_MARKSWEEP
  VGarbageCollector* gc;
#endif
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
#define InterpCurrentCodeElement(interp) \
  (ItemList_Get(InterpCodeList(interp), InterpCodePointer(interp)))

// CREATE/FREE FUNCTIONS ---------------------------------------

VInterp* InterpCreate()
{
  VInterp* interp = MemAlloc(sizeof(VInterp));
  InterpGlobalVars(interp) = ItemList_Create();
  InterpStack(interp) = ItemList_Create();
  InterpCallStack(interp) = ContextList_Create();
  interp->numContextCalls = 0;
#ifdef GC_MARKSWEEP
  interp->gc = GCCreate();
#endif
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

#ifdef GC_MARKSWEEP
  ListFree(InterpGlobalVars(interp));
  ListFree(InterpStack(interp));
  GCFree(interp->gc);
#endif

#ifdef GC_REFCOUNT
  ListGC(InterpGlobalVars(interp));
  ListGC(InterpStack(interp));
#endif

  // Free callstack.
  // This list is not handled by the garbage collector.
  ListFree(InterpCallStack(interp));

  // Free interpreter struct.
  MemFree(interp);
}

#ifdef GC_MARKSWEEP
void InterpGC(VInterp* interp)
{
  GCMarkChildren(InterpGlobalVars(interp));
  GCMarkChildren(InterpStack(interp));
  GCSweep(interp->gc);
}
#endif

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

//#define InterpSetGlobalVar(interp, index, item) \
//  ItemList_Set(InterpGlobalVars(interp), index, item)

void InterpSetGlobalVar(VInterp* interp, VIndex index, VItem* newItem)
{
#ifdef GC_REFCOUNT
  VItem* oldItem = InterpGetGlobalVar(interp, index);
  if (NULL != oldItem) 
    ItemGC(oldItem);
  ItemIncrRefCount(newItem);
#endif
  ItemList_Set(InterpGlobalVars(interp), index, newItem);
}

// DATA STACK --------------------------------------------------

// Push an item onto the data stack.
#ifdef GC_REFCOUNT_STACK
  void InterpPush(VInterp* interp, VItem* item)
  {
    ItemIncrRefCount(item);
    ItemList_Push(InterpStack(interp), item);
  }
#else
  #define InterpPush(interp, item) ItemList_Push(InterpStack(interp), item)
#endif

// Pop an item off the data stack.
#ifdef GC_REFCOUNT_STACK
  VItem* InterpPop(VInterp* interp)
  {
    VItem* item = ItemList_Pop(InterpStack(interp));
    ItemGC(item); // scary prospect
    return item;
  }
#else
  #define InterpPop(interp) ItemList_Pop(InterpStack(interp))
#endif

// Increments the refcount of items indexed from the top of the stack.
// That is the end of the list. 0 = last item, 1 = next last item.
// Called from primfuns to maintain refcount.
#ifdef GC_REFCOUNT_STACK
  void InterpStackItemIncrRefCount(VInterp* interp, VIndex stackIndex)
  {
    VItem* item = ItemList_Get(
      InterpStack(interp), 
      ListLength(InterpStack(interp)) - 1 - stackIndex);
    ItemIncrRefCount(item);
  }
#else
  #define InterpStackItemIncrRefCount(interp, stackIndex)
#endif

void InterpClearStack(VInterp* interp)
{
  // With refcount gc:
  while (ListLength(InterpStack(interp)) > 0) 
    InterpPop(interp);

  // With mark/sweep gc you can do:
  //ListLength(InterpStack(interp)) = 0;
}

// CONTEXT HANDLING --------------------------------------------

// Push root context with codeList.
// Free code list with: ListGC(codeList) (if using GC_REFCOUNT)
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
#ifdef GC_MARKSWEEP
  GCAddObj(interp->gc, ObjCast(codeList));
#endif

  InterpInit(interp, codeList);
  InterpEvalSlice(interp, 0);

#ifdef GC_MARKSWEEP
   InterpGC(interp);
#endif

#ifdef GC_REFCOUNT
  ListGC(codeList);
#endif
}

void InterpEvalTest(VInterp* interp, VList* codeList)
{
#ifdef GC_MARKSWEEP
  GCAddObj(interp->gc, ObjCast(codeList));
#endif

  InterpInit(interp, codeList);
  while ( ! InterpEvalSlice(interp, 100) ) { };

#ifdef GC_MARKSWEEP
   InterpGC(interp);
#endif

#ifdef GC_REFCOUNT
  ListGC(codeList);
#endif
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
    
    //PrintDebugStrNum("CODE POINTER: ", InterpCodePointer(interp));

    // Exit stackframe if the code in the current 
    // context has finished executing.
    if (InterpCodePointer(interp) >= ListLength(InterpCodeList(interp)))
    {
      //PrintDebugStrNum("EXIT CONTEXT: ", InterpCallStackIndex(interp));

      // Pop stackframe.
      InterpPopContext(interp);

      // Exit run loop if this was the last stackframe.
      if (InterpCallStackIndex(interp) < 0) 
        interp->run = FALSE;

      goto Next;
    }

    // Get current element in the code list.
    element = InterpCurrentCodeElement(interp);

    //PrintBinaryULong(element->value.bits);

    if (IsPrimFun(element))
    {
      primFun = ItemPrimFun(element);
      //PrintDebugStrNum("PrimFun: ", primFun);
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

      //PrintDebug("PUSH SYMBOL VALUE");
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
