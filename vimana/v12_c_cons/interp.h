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
  VItem* localVars;
}
VContext;

typedef struct __VInterp
{
  int       run; 
  VMem*     memory;
  VItem*    stack;
  VContext* context;
  int       callstackSize;
  void*     callstack;
}
VInterp;

VInterp* InterpNew(int callstackSize, int memSize)
{
  VInterp* interp = SysAlloc(sizeof(VInterp) + callstackSize);
  interp->callstack = interp + sizeof(VInterp);
  interp->callstackSize = callstackSize;
  interp->memory = MemNew(memSize);
  interp->stack = NULL;
  interp->context = NULL;
  return interp;
}

void InterpFree(VInterp* interp)
{
  MemFree(interp->memory);
  SysFree(interp);
}

void InterpPush(VInterp* interp, VItem* item)
{
  if (NULL == interp->stack)
  {
    interp->stack = item;
    ItemSetNext(item, 0);
  }
  else
  {
    MemCons(interp->memory, item, interp->stack);
  }
}

void InterpPop(VInterp* interp)
{
  if (NULL != interp->stack)
  {
    if (ItemNext(interp->stack))
    {
      VItem* next = MemItemAddr(interp->memory, ItemNext(interp->stack));
      interp->stack = next;
    }
    else
    {
      interp->stack = NULL;
    }
  }
}

void InterpPushContext(VInterp* interp, VItem* code)
{
  if (NULL == interp->context)
  {
    interp->context = interp->callstack;
    interp->context->prev = NULL;
  }
  else
  {
    VContext* nextContext = interp->contex + sizeof(VContext);
    if (nextContext + sizeof(VContext) > callstackSize)
    {
      printf("OUT OF STACK MEMORY\n");
      exit(0);
    }
    nextContext->prev = interp->contex;
    interp->contex = nextContext;
  }

  interp->contex->code = code;
  interp->contex->instruction = code;
}

void InterpPopContext(VInterp* interp)
{
  if (NULL != interp->context)
  {
    interp->contex = interp->context->prevContext;
  }
}

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
