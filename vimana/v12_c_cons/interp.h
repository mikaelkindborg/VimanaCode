/*
File: interp.h
Author: Mikael Kindborg (mikael@kindborg.com)

Interpreter data structures and functions.
*/

typedef struct __VContext
{
  struct __VContext* prev;
  VItem* code;
  VItem* instruction;
  // VItem* localVars;
}
VContext;

typedef struct __VInterp
{
  int       run; 

  VMem*     itemMem;

  int       dataStackSize;
  int       dataStackTop;
  VItem**   dataStack;

  int       callStackByteSize;
  VContext* callStackTop;
  void*     callStack;
}
VInterp;

VInterp* InterpNew(int dataStackSize, int callStackByteSize, int memByteSize)
{
  int dataStackByteSize = dataStackSize * sizeof(VItem*);
  VInterp* interp = SysAlloc(sizeof(VInterp) + dataStackByteSize + callStackByteSize);

  interp->dataStack = (void*)interp + sizeof(VInterp);
  interp->dataStackSize = dataStackSize;
  interp->dataStackTop = -1;

  interp->callStack = interp + sizeof(VInterp) + dataStackByteSize;
  interp->callStackByteSize = callStackByteSize;
  interp->callStackTop = NULL;

  interp->itemMem = MemNew(memByteSize);

  return interp;
}

void InterpFree(VInterp* interp)
{
  MemFree(interp->itemMem);
  SysFree(interp);
}

void InterpPush(VInterp* interp, VItem* item)
{
  ++ interp->dataStackTop;

  if (interp->dataStackTop >= interp->dataStackSize)
  {
    GuruMeditation(DATA_STACK_OVERFLOW);
  }
  
  interp->dataStack[interp->dataStackTop] = item;
}

VItem* InterpPop(VInterp* interp)
{
  if (interp->dataStackTop < 0)
  {
    GuruMeditation(DATA_STACK_IS_EMPTY);
  }

  return interp->dataStack[interp->dataStackTop--];
}

void InterpPushContext(VInterp* interp, VItem* code)
{
  if (NULL == interp->callStackTop)
  {
    interp->callStackTop = interp->callStack;
    interp->callStackTop->prev = NULL;
  }
  else
  {
    VContext* next = (void*)interp->callStackTop + sizeof(VContext);
    void* maxSize = interp->callStack + interp->callStackByteSize;
    if ((void*)next + sizeof(VContext) >= maxSize)
    {
      GuruMeditation(CALL_STACK_OVERFLOW);
    }
    next->prev = interp->callStackTop;
    interp->callStackTop = next;
  }

  interp->callStackTop->code = code;
  interp->callStackTop->instruction = code;
}

void InterpPopContext(VInterp* interp)
{
  if (NULL == interp->callStackTop)
  {
    GuruMeditation(CALL_STACK_IS_EMPTY);
  }

  interp->callStackTop = interp->callStackTop->prev;
}

/*
int InterpEvalSlice(register VInterp* interp, register int sliceSize);

void InterpEval(VInterp* interp, VItem* code)
{
  InterpPushContext(interp, code);
  InterpEvalSlice(interp, 0);
  // TODO: GC
}

// Evaluate a slice of code. 
// sliceSize specifies the number of instructions to execute.
// sliceSize 0 means eval as one slice until program ends.
// Returns done flag (TRUE = done, FALSE = not done).
int InterpEvalSlice(register VInterp* interp, register int sliceSize)
{
  register VItem*  instruction;
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

    // Exit stackframe if the current context has finished executing.
    if (NULL == ItemNext(interp->context->instruction))
    {
      InterpPopContext(interp);

      // Exit run loop if this was the last stackframe.
      if (NULL == interp->context)
      {
        interp->run = FALSE;
      }

      goto Next;
    }

    // Get current instruction.
    instruction = interp->context->instruction;

    if (TypePrimFun == ItemType(instruction))
    {
      primFun = ItemData(instruction);
      #include "primfuns_gen.h"
      goto Next;
    }

    if (TypeSymbol == ItemType(instruction))
    {
      HÄR ÄR JAG
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

Next:
    interp->context->instruction = MemItemNext(interp->memory, instruction); // TODO
  } // while

Exit:
  return ! interp->run;
}
*/
