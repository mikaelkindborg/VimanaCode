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
  ItemList_Init(InterpGlobalVars(interp));
  ItemList_Init(InterpStack(interp));
  ContextList_Init(InterpCallStack(interp));
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
  ItemList_Set(InterpGlobalVars(interp), index, item)

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
    VItem* item = ItemList_GetRaw(list, i);
    if (IsObj(item))
    {
      VBool isGlobal = ItemListContains(InterpGlobalVars(interp), item);
      if (isGlobal)
      {
        continue; // Keep item
      }

      VBool isOnStack = ItemListContains(InterpStack(interp), item);
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
#define InterpPush(interp, item) ItemList_Push(InterpStack(interp), item)

// Pop an item off the data stack.
#define InterpPop(interp) ItemList_Pop(InterpStack(interp))

// CONTEXT HANDLING --------------------------------------------

void InterpInit(VInterp* interp)
{
  interp->run = TRUE;
  InterpCallStackIndex(interp) = -1;
  ListLength(InterpCallStack(interp)) = 0;
}

void InterpPushContext(VInterp* interp, VList* codeList)
{
  VBool isTailCall = 
    (1 + InterpCodePointer(interp)) >= 
    ListLength(InterpCodeList(interp));
  if (isTailCall)
  {
    //PrintDebugStrNum("TAILCALL CONTEXT: ", InterpCallStackIndex(interp));
  }
  else
  {
    ListPushNewElement(InterpCallStack(interp));
    ++ InterpCallStackIndex(interp);
    //PrintDebugStrNum("ENTER CONTEXT: ", InterpCallStackIndex(interp));
  }

  InterpCodeList(interp) = codeList;
  InterpCodePointer(interp) = -1;
}

void InterpPushContextBasic(VInterp* interp, VList* codeList)
{
  ListPushNewElement(InterpCallStack(interp));
  ++ InterpCallStackIndex(interp);
  InterpCodeList(interp) = codeList;
  InterpCodePointer(interp) = -1;
  //PrintDebugStrNum("ENTER CONTEXT: ", InterpCallStackIndex(interp));
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
  InterpPushContextBasic(interp, codeList);

  while (interp->run)
  {
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

  InterpTinyGarbageCollect(interp, codeList);
  ListFree(codeList);
}
