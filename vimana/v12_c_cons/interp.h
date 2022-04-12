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

  VMem*     mem;

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
VPrimFunPtr LookupPrimFunPtr(int index);

//#define InterpMem(interp) ((interp)->mem)

VInterp* InterpNewWithSize(
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

  interp->mem = MemNew(memSize);

  return interp;
}

VInterp* InterpNew()
{
  return InterpNewWithSize(
    100, // dataStackSize, 
    100, // globalVarsSize,
    100, // callStackSize
    1000 // memSize
  );
}

void InterpFree(VInterp* interp)
{
  MemSweep(interp->mem);
  MemFree(interp->mem);
  SysFree(interp);
}

void InterpGC()
{
  //MemMark(interp->mem, GSymbolTable);
  //MemMark(datastack);
  //MemMark(globavars);
  //MemMark(callstack); // Walk from top and mark localvars
}

void InterpPush(VInterp* interp, VItem *item)
{
  ++ interp->dataStackTop;

  if (interp->dataStackTop >= interp->dataStackSize)
  {
    GURU(DATA_STACK_OVERFLOW);
  }
  
  // Copy item
  interp->dataStack[interp->dataStackTop] = *item;
}

VItem* InterpPop(VInterp* interp)
{
  if (interp->dataStackTop < 0)
  {
    GURU(DATA_STACK_IS_EMPTY);
  }

  return & (interp->dataStack[interp->dataStackTop --] );
}

void InterpPushContext(VInterp* interp, VItem* code, int newContext)
{
  if (NULL == interp->callStackTop)
  {
    interp->callStackTop = interp->callStack;
    interp->callStackTop->prev = NULL;
  }
  else 
  if (newContext)
  {
    VContext* next = (void*)interp->callStackTop + sizeof(VContext);
    void* maxSize = interp->callStack + (interp->callStackSize * sizeof(VContext));
    if ((void*)next + sizeof(VContext) >= maxSize)
    {
      GURU(CALL_STACK_OVERFLOW);
    }
    next->prev = interp->callStackTop;
    interp->callStackTop = next;
  }

  interp->callStackTop->code = code;
  interp->callStackTop->instruction = code;
}

void InterpPopContext(VInterp* interp)
{
  printf("POP CONTEXT\n");

  if (NULL == interp->callStackTop)
  {
    GURU(CALL_STACK_IS_EMPTY);
  }

  interp->callStackTop = interp->callStackTop->prev;
}

void InterpSetGlobalVar(VInterp* interp, int index, VItem* item)
{
  if (index < interp->globalVarsSize)
    (interp->globalVars)[index] = *item;
  else
    GURU(GLOBALVARS_OVERFLOW);
}

#define InterpGetGlobalVar(interp, index) (& (((interp)->globalVars)[index]))

int InterpEvalSlice(register VInterp* interp, register int sliceSize);

void InterpEval(VInterp* interp, VItem* code)
{
  InterpPushContext(interp, MemItemFirst(interp->mem, code), 1);
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
      if (IsTypePrimFun(instruction))
      {
        int primfunId = instruction->intNum;
        VPrimFunPtr fun = LookupPrimFunPtr(primfunId);
        fun(interp);
      }
      else
      if (IsTypePushable(instruction))
      {
        InterpPush(interp, instruction);
      }
      else
      if (IsTypeSymbol(instruction))
      {
        VItem* value = InterpGetGlobalVar(interp, instruction->intNum);
        //if (!IsTypeNone(value))
        if (IsTypeFun(value))
        {
          InterpPushContext(interp, MemItemFirst(interp->mem, value), instruction->next);
          goto Next;
        }
        else
        {
          InterpPush(interp, value);
        }
      }
      else
      {
        printf("Instruction type: %i\n", ItemType(instruction)); 
        GURU(INTERP_UNEXPECTED_TYPE);
      }
    }
    else
    {
      InterpPopContext(interp);

      // Exit if this was the last stackframe.
      if (NULL == interp->callStackTop)
      {
        printf("EXIT INTERP LOOP\n");
        interp->run = FALSE;
        goto Exit;
      }
    }

    printf("STACK: ");
    for (int i = 0; i <= interp->dataStackTop; ++ i)
    {
      MemPrintItem(interp->mem, &(interp->dataStack[i]));
      printf(" ");
    }
    printf("\n");

    interp->callStackTop->instruction = 
      MemItemNext(interp->mem, interp->callStackTop->instruction);

Next:;

  }
  // while

Exit:
  return ! interp->run;
}
