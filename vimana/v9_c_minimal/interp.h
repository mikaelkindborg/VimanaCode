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
  long    numContextCalls;
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
  PrintLine("CONTEXT CALLS:");
  PrintNum(interp->numContextCalls);
  PrintNewLine();

  // Free lists.
  ObjGC(InterpGlobalVars(interp));
  ObjDeref(InterpStack(interp));
  ObjDeref(InterpCallStack(interp));

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

// TINY GC -----------------------------------------------------

void ListGC(VList* list)
{
  for (int i = 0; i < ListLength(list); ++i)
  {
    VItem* item = ItemList_GetRaw(list, i);

      // Free item
      if (IsList(item))
      {
        InterpTinyGC(interp, ItemList(item));
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

void ItemIncrementRefCounter(VItem* item)
{
  if (IsObj(item))
  {
    VObj* obj = ItemObj(item);
    ++ obj->refCount;
  }
}

void ItemGC(VItem* item)
{
  if (IsObj(item))
  {
    ObjGC(ItemObj(item));
  }
}

void ObjGC(VObj* obj)
{
  -- obj->refCount;
  if (obj->refCounter <= 0)
  {
  if (TypeString == obj->type)
  {
    StringFree((VString*) obj);
  }
  else
  if (TypeList == obj->type || TypeFun == obj->type)
  {
    ListGC((VList*) obj);
    ListFree(ItemList(item));
  }
  else
  {
    PrintLine("ALERT - UNKNOWN TYPE IN ObjGC");
    exit(0);
  }
}

void InterpGCObj(VInterp* interp, VObj* obj)
{
  // TODO: Check String/List. New type VObj.
}

void InterpGCList(VInterp* interp, VList* list)
{
  for (int i = 0; i < ListLength(list); ++i)
  {
    VItem* item = ItemList_GetRaw(list, i);
    if (IsObj(item))
    {
      VList* obj = ItemObj(item);
      -- obj->refCount;
      if (obj->refCount > 0)
      {
        continue; // Keep item
      }

      // Free item
      if (IsList(item))
      {
        InterpTinyGC(interp, ItemList(item));
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

void InterpSetGlobalVar(VInterp* interp, VIndex index, VItem* item)
{
  VItem* currentItem = InterpGetGlobalVar(interp, index);
  if (NULL != currentItem)
  { 
    ItemDecrementRefCounter(currentItem);
  }
  ItemIncrementRefCounter(item);
  ItemList_Set(InterpGlobalVars(interp), index, item);
}

// DEALLOCATE UNREFRENCED ITEMS --------------------------------

// This version of Vimana has manual memory management, with the
// assistance of this function that deallocates all items not
// referenced in the global var table.

// Check if an item is in the global var list.
// Does not handle circular lists.

// TODO: Rewrite using InterpTinyCG()

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
//#define InterpPush(interp, item) ItemList_Push(InterpStack(interp), item)

void InterpPush(VInterp* interp, VItem* item)
{
  if (IsObj(item))
  {
    VList* obj = ItemObj(item);
    ++ obj->refCount;
  }

  ItemList_Push(InterpStack(interp), item);
}

// Pop an item off the data stack.
//#define InterpPop(interp) ItemList_Pop(InterpStack(interp))

VItem* InterpPop(VInterp* interp)
{
  VItem* item = ItemList_Pop(InterpStack(interp));
  if (IsObj(item))
  {
    VList* obj = ItemObj(item);
    -- obj->refCount;
  }

  ItemList_Push(InterpStack(interp), item);
}

// CONTEXT HANDLING --------------------------------------------

void InterpInit(VInterp* interp)
{
  interp->run = TRUE;
  InterpCallStackIndex(interp) = -1;
  ListLength(InterpCallStack(interp)) = 0;
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
      ListPushNewElement(InterpCallStack(interp));
    //PrintDebugStrNum("ENTER CONTEXT: ", InterpCallStackIndex(interp));
  }

  InterpCodeList(interp) = codeList;
  InterpCodePointer(interp) = -1;
}

void InterpPushRootContext(VInterp* interp, VList* codeList)
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
  InterpPushRootContext(interp, codeList);

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
