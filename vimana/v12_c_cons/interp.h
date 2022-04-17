/*
File: interp.h
Author: Mikael Kindborg (mikael@kindborg.com)

Interpreter data structures and functions.
*/

typedef struct __VContext
{
  struct __VContext* prev;
  struct __VContext* funContext;
  VItem*             code;
  VItem*             instruction;
  VItem              localVars[4];
}
VContext;

#define ContextSetLocalVar(context, index, itempointer) \
  ((context)->localVars[index] = *(itempointer))

#define ContextGetLocalVar(context, index) \
  (& ((context)->localVars[index]))

// Mind control for hackers

// mindfulness = ta kontroll över sitt tänkande, sina tankar

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
  VContext* callStackTop;    // Current context
  VContext* callStackTopFun; // Closest function call
  void*     callStack;
}
VInterp;

typedef struct __VInterp VInterp;
typedef void (*VPrimFunPtr) (VInterp*);

VPrimFunPtr LookupPrimFunPtr(int index);

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

  GSymbolTableInit();

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
  GSymbolTableRelease();
}

void InterpGC()
{
  //MemMark(interp->mem, GSymbolTable);
  //MemMark(datastack);
  //MemMark(globavars);
  //MemMark(callstack); // Walk from top and mark localvars
}

void InterpStackPush(VInterp* interp, VItem *item)
{
  ++ interp->dataStackTop;

  if (interp->dataStackTop >= interp->dataStackSize)
  {
    GURU(DATA_STACK_OVERFLOW);
  }
  
  // Copy item
  interp->dataStack[interp->dataStackTop] = *item;
}

VItem* InterpStackPop(VInterp* interp)
{
  if (interp->dataStackTop < 0)
  {
    GURU(DATA_STACK_IS_EMPTY);
  }

  return & (interp->dataStack[interp->dataStackTop --] );
}

#ifdef OPTIMIZE

#define InterpStackAt(interp, indexFromEnd) \
  ( & ((interp)->dataStack[(interp)->dataStackTop - indexFromEnd]) )

#define InterpStackTop(interp) InterpStackAt(interp, 0)

#else

VItem* InterpStackAt(VInterp* interp, int indexFromEnd)
{
  return & (interp->dataStack[interp->dataStackTop - indexFromEnd]);
}

VItem* InterpStackTop(VInterp* interp)
{
  return InterpStackAt(interp, 0);
}

#endif

void InterpPushContextImpl(VInterp* interp, VItem* code, int funcall)
{
  if (NULL == interp->callStackTop)
  {
    // Set root context
    interp->callStackTop = interp->callStack;
    interp->callStackTop->prev = NULL;

    // The root context has local vars
    interp->callStackTop->funContext = interp->callStackTop;
  }
  else
  {
    // Push new context if this is not the last instruction
    if (interp->callStackTop->instruction)
    {
      VContext* next = (void*)interp->callStackTop + sizeof(VContext);

      void* maxSize = interp->callStack + (interp->callStackSize * sizeof(VContext));

      if ((void*)next + sizeof(VContext) >= maxSize)
      {
        GURU(CALL_STACK_OVERFLOW);
      }

      // Make new context top
      next->prev = interp->callStackTop;
      interp->callStackTop = next;

      // If function call the context has local vars
      if (funcall)
      {
        next->funContext = next;
      }
      else
      {
        next->funContext = next->prev->funContext;
      }
    }
  }

  // Set first instruction in new frame
  interp->callStackTop->code = code;
  interp->callStackTop->instruction = MemItemFirst(interp->mem, code);
}

#define InterpPushContext(interp, code) \
  InterpPushContextImpl(interp, code, 0)

#define InterpPushFunContext(interp, code) \
  InterpPushContextImpl(interp, code, 1)

void InterpPopContext(VInterp* interp)
{
  //printf("POP CONTEXT\n");

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
    // Count slices if a sliceSize is specified.
    if (sliceSize)
    {
      if (sliceSize > sliceCounter)
        ++ sliceCounter;
      else
        goto Exit; // Exit loop
    }

    // Evaluate current instruction.
    instruction = interp->callStackTop->instruction;

    if (NULL != instruction)
    {
      // Advance instruction for next loop
      interp->callStackTop->instruction = 
        MemItemNext(interp->mem, interp->callStackTop->instruction);

      if (IsTypePrimFun(instruction))
      {
        #ifdef OPTIMIZE
        VPrimFunPtr fun = instruction->primFunPtr;
        fun(interp);
        #else
        int primFunId = instruction->intNum;
        VPrimFunPtr fun = LookupPrimFunPtr(primFunId);
        fun(interp);
        #endif
      }
      else
      if (IsTypePushable(instruction))
      {
        InterpStackPush(interp, instruction);
      }
      else
      if (IsTypeSymbol(instruction))
      {
        VItem* value = InterpGetGlobalVar(interp, instruction->intNum);
        //if (!IsTypeNone(value))
        if (IsTypeFun(value))
        {
          InterpPushFunContext(interp, value);
        }
        else
        {
          InterpStackPush(interp, value);
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
        interp->run = FALSE;
        goto Exit; // Exit loop
      }
    }
  }
  // while

Exit:
  return ! interp->run;
}
