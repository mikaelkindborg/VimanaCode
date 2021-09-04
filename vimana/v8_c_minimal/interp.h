
// DECLARATIONS ------------------------------------------------

// CONTEXT -----------------------------------------------------

// A context is a stackframe on the callstack.
// It is used to track code execution.
typedef struct __VContext
{
  VList* codeList;
  VIndex codePointer;
}
VContext;

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

typedef struct __VInterp
{
  VList     globalVars;        // List of global variable values
  VList     stack;             // The data stack
  VList     callstack;         // Callstack with context frames
  VIndex    callstackIndex;    // Index of current frame
  VBool     run;               // Run flag
}
VInterp;

// ACCESSORS ---------------------------------------------------

#define InterpGlobalVars(interp) \
  (& ((interp)->globalVars))
#define InterpStack(interp) \
  (& ((interp)->stack))
#define InterpCallStack(interp) \
  (& ((interp)->callstack))
#define InterpCallStackIndex(interp) \
  ((interp)->callstackIndex)
#define InterpContext(interp) \
  ((VContext*)ListGet(InterpCallStack(interp), InterpCallStackIndex(interp)))
#define InterpCodeList(interp) \
  (InterpContext(interp)->codeList)
#define InterpCodePointer(interp) \
  (InterpContext(interp)->codePointer)
#define InterpCurrentCodeElement(interp) \
  ((VItem*)ListGet(InterpCodeList(interp), InterpCodePointer(interp)))

// CREATE/FREE FUNCTIONS ---------------------------------------

VInterp* InterpCreate()
{
  VInterp* interp = MemAlloc(sizeof(VInterp));
  ListInit(InterpGlobalVars(interp), sizeof(VItem));
  ListInit(InterpStack(interp), sizeof(VItem));
  ListInit(InterpCallStack(interp), sizeof(VContext));
  return interp;
}

void InterpFree(VInterp* interp)
{
  // Free lists.
  ListDeallocItems(InterpGlobalVars(interp));
  ListDeallocItems(InterpStack(interp));
  ListDeallocItems(InterpCallStack(interp));

  // Free interpreter struct.
  MemFree(interp);
}

// GLOBAL VARIABLES --------------------------------------------

#define InterpSetGlobal(interp, index, item) \
  ListSet(InterpGlobalVars(interp), index, value)

#define InterpGetGlobal(interp, index) \
  ListGet(InterpGlobalVars(interp), index)

// DATA STACK --------------------------------------------------

// Push an item onto the data stack.
#define InterpPush(interp, item) ListPush(InterpStack(interp), item)

// Pop an item off the data stack.
#define InterpPop(interp) ListPop(InterpStack(interp))

// CONTEXT HANDLING --------------------------------------------

void InterpInit(VInterp* interp)
{
  interp->run = TRUE;
  InterpCallStackIndex(interp) = -1;
  ListLength(InterpCallStack(interp)) = 0;
}

void InterpPushContext(VInterp* interp, VList* codeList)
{
  // TODO Tailcall
  VContext context;
  context.codeList = codeList;
  context.codePointer = -1;
  ListPush(InterpCallStack(interp), &context);
  ++ InterpCallStackIndex(interp);
  //InterpCodeList(interp) = codeList;
  //InterpCodePointer(interp) = -1;
}

#define InterpPopContext(interp) \
  (-- InterpCallStackIndex(interp))

// INTERPRETER LOOP --------------------------------------------

void InterpRun(register VInterp* interp, VList* codeList)
{
  register VItem* element;
  register VItem* evalResult;
  register int     primFun;

  // Initialize interpreter state and create root context.
  InterpInit(interp);
  InterpPushContext(interp, codeList);

  while (interp->run)
  {
    // Increment code pointer.
    ++ InterpCodePointer(interp);

    // Exit stackframe if the code in the current 
    // context has finished executing.
    if (InterpCodePointer(interp) >= ListLength(InterpCodeList(interp)))
    {
      PrintDebugStrNum("EXIT CONTEXT: ", InterpCallStackIndex(interp));

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

    if (IsPrimFun(*element))
    {
      primFun = ItemPrimFun(*element);
      //PrintDebugStrNum("PrimFun: ", primFun);
      #include "primfuns.h"
      goto Next;
    }
/*
    if (IsSymbol(element))
    {
      // Find symbol value in global env.
      // Symbols evaluate to themselves if unbound.
      symbolValue = InterpEvalSymbol(interp, element);
      
      // If it is a function, call it.
      if (IsFun(evalResult))
      {
        InterpPushContext(interp, (VList*) symbolValue.value.obj);
        goto Next;
      }

      // If not a function, push the symbol value.
      InterpPush(interp, evalResult);
      goto Next;
    }
*/
    // If none of the above, push the element.
    InterpPush(interp, element);
Next:;
  } // while
}
