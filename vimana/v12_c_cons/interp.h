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

// Mind control for hackers

typedef struct __VInterp
{
  int       run; 

  VMem*     itemMem;

  int       dataStackSize;
  int       dataStackTop;
  VItem*    dataStack;

  int       globalVarsSize;
  VItem*    globalVars;

  int       callStackSize;
  VContext* callStackTop;
  void*     callStack;
}
VInterp;

typedef void (*VPrimFunPtr) (VInterp*);

VItem* GSymbolTable;

VInterp* InterpNew(
  int dataStackSize, int globalVarsSize, 
  int callStackSize, int memSize)
{
  int dataStackByteSize = dataStackSize * sizeof(VItem);
  int globalVarsByteSize = globalVarsSize * sizeof(VItem);
  int callStackByteSize = callStackSize * sizeof(VContext);

  VInterp* interp = SysAlloc(
    sizeof(VInterp) + dataStackByteSize + 
    globalVarsByteSize + callStackByteSize);

  interp->dataStack = (void*)interp + sizeof(VInterp);
  interp->dataStackSize = dataStackSize;
  interp->dataStackTop = -1;

  interp->globalVars = (void*)interp->dataStack + dataStackByteSize;
  interp->globalVarsSize = globalVarsSize;

  interp->callStack = (void*)interp->globalVars + globalVarsByteSize;
  interp->callStackSize = callStackSize;
  interp->callStackTop = NULL;

  interp->itemMem = MemNew(memSize);

  GSymbolTable = ArrayNew(20);

  return interp;
}

void InterpFree(VInterp* interp)
{
  MemFree(interp->itemMem);
  SysFree(interp);
}

void InterpGC()
{
  //MemMark(interp->itemMem, GSymbolTable);
  //MemMark(datastack);
  //MemMark(globavars);
  //MemMark(callstack); // Walk from top and mark localvars
}

void InterpPush(VInterp* interp, VItem* item)
{
  ++ interp->dataStackTop;

  if (interp->dataStackTop >= interp->dataStackSize)
  {
    GuruMeditation(DATA_STACK_OVERFLOW);
  }
  
  // Copy item
  interp->dataStack[interp->dataStackTop] = *item;
}

VItem* InterpPop(VInterp* interp)
{
  if (interp->dataStackTop < 0)
  {
    GuruMeditation(DATA_STACK_IS_EMPTY);
  }

  return & (interp->dataStack[interp->dataStackTop --]);
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
    void* maxSize = interp->callStack + (interp->callStackSize * sizeof(VContext));
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
primitive_add()
{
  VObj* num2 = InterpPop();
  VObj* num1 = InterpPop();
  VObj* result = InterpAlloc();
  int sum = num1->data + num2->data;
  ObjSetIntNum(result, sum);
  InterpPush(result);
}
*/

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
int InterpEvalSlice(VInterp* interp, int sliceSize)
{
  VItem*  instruction;
  VItem*  symbolValue;
  int     primFun;
  int     sliceCounter = 0;

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

    // Evaluate current instruction.
    instruction = interp->callStackTop->instruction;

    if (NULL != instruction)
    {
      InterpPush(interp, instruction);

      if (IsTypePrimFun(instruction))
      {
        printf("primfun\n");
        int primfunId = ItemData(instruction);
        //VPrimFunPtr fun = 
        //fun(interp, instruction);
      }

      /*
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
      }
      */
    }
    else
    {
      // Pop stackframe.
      InterpPopContext(interp);

      // Exit if this was the last stackframe.
      if (NULL == interp->callStackTop)
      {
        interp->run = FALSE;
        goto Exit;
      }
    }

Next:
    interp->callStackTop->instruction = 
      MemItemNext(interp->itemMem, instruction);
  }
  // while

Exit:
  return ! interp->run;
}
