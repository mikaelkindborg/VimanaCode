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

// INTERPRETER -------------------------------------------------

typedef struct __VInterp
{
  VList  globalVars;        // List of global variable values
  VList  stack;             // The data stack
  VList  callstack;         // Callstack with context frames
  VIndex callstackIndex;    // Index of current frame
  VBool  run;               // Run flag
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
  Print("STACK:");
  PrintList(InterpStack(interp));
  PrintNewLine();
  Print("GLOBALS:");
  PrintList(InterpGlobalVars(interp));
  PrintNewLine();

  // Free lists.
  ListDeallocArrayBufDeep(InterpGlobalVars(interp));
  ListDeallocArrayBufDeep(InterpStack(interp));
  ListDeallocArrayBuf(InterpCallStack(interp));
  /*
  VList* deallocatedItems = ListCreate(sizeof(VItem));
  ListDeallocArrayBufDeepSafe(InterpGlobalVars(interp), deallocatedItems);
  ListDeallocArrayBufDeepSafe(InterpStack(interp), deallocatedItems);
  ListDeallocArrayBuf(InterpCallStack(interp));
  ListFree(deallocatedItems);
  */

  // Free interpreter struct.
  MemFree(interp);
}

// GLOBAL VARIABLES --------------------------------------------

#define InterpSetGlobalVar(interp, index, item) \
  ListSet(InterpGlobalVars(interp), index, item)

VItem* InterpGetGlobalVar(VInterp* interp, VIndex index)
{
  if (index < ListLength(InterpGlobalVars(interp)))
  {
    VItem* item = ListGet(InterpGlobalVars(interp), index);
    if (!IsVirgin(item))
      return item;
  }
  
  // No value found
  return NULL;
}

// DEALLOCATE UNREFRENCED ITEMS --------------------------------

// This version of Vimana has manual memory management, with the
// assistance of this function that deallocates all items not
// referenced in the global var table.

// Check if an item is in the global var list.
// Does not handle circular lists.
void InterpTinyGarbageCollect(VInterp* interp, VList* list)
{
  for (int i = 0; i < ListLength(list); ++i)
  {
    VItem* item = (VItem*) ListItemPtr(list, i);
    if (IsObj(item))
    {
      VBool isGlobal = ListContainsItem(InterpGlobalVars(interp), item);
      if (isGlobal)
      {
        continue; // Keep item
      }

      VBool isOnStack = ListContainsItem(InterpStack(interp), item);
      if (isOnStack)
      {
        continue; // Keep item
      }

      // Free item
      if (IsList(item))
      {
        InterpTinyGarbageCollect(interp, ItemList(item));
        ListFree(ItemList(item));
      }
      else
      if (IsString(item))
      {
        StringFree(ItemObj(item));
      }
    } 
  }
}

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
  register VItem*  element;
  register VItem*  symbolValue;
  register VIndex  primFun;

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

    if (IsPrimFun(element))
    {
      primFun = ItemPrimFun(element);
      //PrintDebugStrNum("PrimFun: ", primFun);
      #include "primfuns.h"
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
        InterpPushContext(interp, ItemObj(symbolValue));
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

  PrintDebug("FOO1");
  InterpTinyGarbageCollect(interp, codeList);

  PrintDebug("FOO2");
  ListFree(codeList);

  PrintDebug("FOO3");
}
