
// DECLARATIONS ------------------------------------------------

// CONTEXT -----------------------------------------------------

// A context is a stackframe on the callstack.
// It is used to track code execution.
typedef struct __VmContext
{
  VmList* codeList;
  VmIndex codePointer;
}
VmContext;

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

typedef struct __VmInterp
{
  VmList     globalVars;        // List of global variable values
  VmList     stack;             // The data stack
  VmList     callstack;         // Callstack with context frames
  VmIndex    callstackIndex;    // Index of current frame
  VmBool     run;               // Run flag
}
VmInterp;

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
  (ListGet(InterpCallStack(interp), InterpCallStackIndex(interp)))
#define InterpCodeList(interp) \
  (InterpContext(interp)->codeList)
#define InterpCodePointer(interp) \
  (InterpContext(interp)->codePointer)
#define InterpCurrentCodeElement(interp) \
  (ListGet(InterpCodeList(interp), InterpCodePointer(interp)))

// CREATE/FREE FUNCTIONS ---------------------------------------

VmInterp* InterpCreate()
{
  VmInterp* interp = MemAlloc(sizeof(VmInterp));
  ListInit(InterpGlobalVars(interp), sizeof(VmItem));
  ListInit(InterpStack(interp), sizeof(VmItem));
  ListInit(InterpCallStack(interp), sizeof(VmContext));
  return interp;
}

void InterpFree(Interp* interp)
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
#define InterpPush(interp, context) ListPush(InterpStack(interp), context)

// Pop an item off the data stack.
#define InterpPop(interp) ListPop(InterpStack(interp))

// CONTEXT HANDLING --------------------------------------------

void InterpInit(Interp* interp)
{
  interp->run = TRUE;
  InterpCallstackIndex(interp) = -1;
}

void InterpPushContext(Interp* interp, VmList* codeList)
{
  ++ InterpCallstackIndex(interp);
  InterpCodeList(interp) = codeList;
  InterpCodePointer(interp) = -1;
}

#define InterpPopContext(interp) \
  (-- InterpCallstackIndex(interp))

// INTERPRETER LOOP --------------------------------------------

void InterpRun(register Interp* interp, List* codeList)
{
  register VmItem     element;
  register VmItem     evalResult;

  // Initialize interpreter state and create root context.
  InterpInit(interp);
  InterpPushContext(codeList);

  while (interp->run)
  {
    // Increment code pointer.
    ++ InterpCodePointer(interp);

    // Exit stackframe if the code in the current 
    // context has finished executing.
    if (InterpCodePointer(interp) >= ListLength(InterpCodeList(interp)))
    {
      PrintDebug("EXIT CONTEXT: %i", callstackIndex);

      // Pop stackframe.
      InterpPopContext(interp);

      // Exit run loop if this was the last stackframe.
      if (InterpCallStackIndex(interp) < 0) 
        interp->run = FALSE;

      goto Next;
    }

    // Get current element in the code list.
    element = InterpCurrentCodeElement(interp);

    if (IsPrimFun(element))
    {
      #include "primfuns.h"
      goto Next;
    }

    if (IsSymbol(element))
    {
      // Find symbol value in global env.
      // Symbols evaluate to themselves if unbound.
      symbolValue = InterpEvalSymbol(interp, element);
      
      // If it is a function, call it.
      if (IsFun(evalResult))
      {
        InterpPushContext(interp, (VmList*) symbolValue.value.obj);
        goto Next;
      }

      // If not a function, push the symbol value.
      InterpPush(interp->stack, evalResult);
      goto Next;
    }

    // If none of the above, push the element.
    ListPush(interp->stack, element);
Next:;
  } // while
}
