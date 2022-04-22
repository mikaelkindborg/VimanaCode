/*
File: interp.h
Author: Mikael Kindborg (mikael@kindborg.com)

Interpreter data structures and functions.
*/

// -------------------------------------------------------------
// Data types and structs
// -------------------------------------------------------------

typedef struct __VStackFrame
{
  VItem* instruction;
}
VStackFrame;

typedef struct __VContext
{
  VItem     localVars[2];
  VSmallInt counter;
}
VContext;

// Mind control for hackers

// mindfulness = ta kontroll över sitt tänkande, sina tankar

typedef struct __VInterp
{
  int          run; 

  VMem*        mem;

  int          globalVarsSize;
  VItem*       globalVars;

  int          dataStackSize;
  int          dataStackTop;
  VItem*       dataStack;

  int          contextStackSize;
  int          contextStackTop;
  VContext*    contextStack;

  int          callStackSize;
  int          callStackTop; // Current context
  VStackFrame* callStack;
}
VInterp;

typedef struct __VInterp VInterp;
typedef void (*VPrimFunPtr) (VInterp*);

VPrimFunPtr LookupPrimFunPtr(int index);

#define InterpStackFrame(interp) ( ((interp)->callStack) + ((interp)->callStackTop) )

// -------------------------------------------------------------
// Interp
// -------------------------------------------------------------

VInterp* InterpNewWithSize(
  int globalVarsSize, int dataStackSize, int contextStackSize, 
  int callStackSize, int memSize)
{
  int globalVarsByteSize = globalVarsSize * sizeof(VItem);
  int dataStackByteSize = dataStackSize * sizeof(VItem);
  int contextStackByteSize = contextStackSize * sizeof(VContext);
  int callStackByteSize = callStackSize * sizeof(VStackFrame);

  VInterp* interp = SysAlloc(
    sizeof(VInterp) + globalVarsByteSize + dataStackByteSize + 
    contextStackByteSize + callStackByteSize);

  interp->globalVars = (void*)interp + sizeof(VInterp);
  interp->globalVarsSize = globalVarsSize;

  interp->dataStack = (void*)interp->globalVars + globalVarsByteSize;
  interp->dataStackSize = dataStackSize;
  interp->dataStackTop = -1;

  interp->contextStack = (void*)interp->dataStack + dataStackByteSize;
  interp->contextStackSize = dataStackSize;
  interp->contextStackTop = -1;

  interp->callStack = (void*)interp->contextStack + contextStackByteSize;
  interp->callStackSize = callStackSize;
  interp->callStackTop = -1;

  interp->mem = MemNew(memSize);

  GSymbolTableInit();

  return interp;
}

VInterp* InterpNew()
{
  return InterpNewWithSize(
    100, // globalVarsSize,
    100, // dataStackSize, 
    100, // contextStackSize, 
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

// -------------------------------------------------------------
// Data stack
// -------------------------------------------------------------

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
  //return interp->dataStack + (interp->dataStackTop --);
}

#ifdef OPTIMIZE

#define InterpStackAt(interp, offsetFromTop) \
  ( & ((interp)->dataStack[(interp)->dataStackTop - offsetFromTop]) )

#define InterpStackTop(interp) InterpStackAt(interp, 0)

#else

VItem* InterpStackAt(VInterp* interp, int offsetFromTop)
{
  return & (interp->dataStack[interp->dataStackTop - offsetFromTop]);
}

VItem* InterpStackTop(VInterp* interp)
{
  return InterpStackAt(interp, 0);
}

#endif

// -------------------------------------------------------------
// Context stack
// -------------------------------------------------------------

void InterpPushContext(VInterp* interp)
{
  ++ interp->contextStackTop;

//printf("push context\n");
  if (interp->contextStackTop >= interp->contextStackSize)
  {
    GURU(CONTEXT_STACK_OVERFLOW);
  }

  interp->contextStack[interp->contextStackTop].counter = 1;
}

void InterpPopContext(VInterp* interp)
{
  if (interp->contextStackTop < 0)
  {
    GURU(CONTEXT_STACK_IS_EMPTY);
  }
  -- interp->contextStackTop;
//printf("pop context\n");
}

void InterpIncrContextCounter(VInterp* interp)
{
  if (interp->contextStackTop > -1)
  {
    ++ (interp->contextStack[interp->contextStackTop].counter);
    //printf("InterpIncrContextCounter: %i\n", interp->contextStack[interp->contextStackTop].counter);
  }
}

void InterpDecrContextCounter(VInterp* interp)
{
  if (interp->contextStackTop > -1)
  {
    -- (interp->contextStack[interp->contextStackTop].counter);

    //printf("InterpDecrContextCounter: %i\n", interp->contextStack[interp->contextStackTop].counter);
    if (interp->contextStack[interp->contextStackTop].counter <= 0)
    {
      InterpPopContext(interp);
    }
  }
}

VContext* InterpContextAt(VInterp* interp, int offsetFromTop)
{
  if (interp->contextStackTop < 0)
  {
    GURU(CONTEXT_STACK_IS_EMPTY);
  }

  //return & (interp->contextStack[interp->contextStackTop - offsetFromTop]);
  return interp->contextStack + (interp->contextStackTop - offsetFromTop);
}

void InterpContextSetLocalVar(VInterp* interp, int index, VItem* item)
{
  VContext* context = InterpContextAt(interp, 0);
  context->localVars[index] = *item;
}

VItem* InterpContextGetLocalVar(VInterp* interp, int index)
{
  VContext* context = InterpContextAt(interp, 0);
  return & (context->localVars[index]);
}

// -------------------------------------------------------------
// Call stack
// -------------------------------------------------------------

void InterpPushStackFrame(VInterp* interp, VItem* code)
{
  VStackFrame* current;

  if (interp->callStackTop < 0)
  {
    ++ interp->callStackTop;
    current = InterpStackFrame(interp);
  }
  else
  {
    current = InterpStackFrame(interp);

    // Check tailcall (are there any instructions left?)
    void* pushContext = current->instruction;
    if (pushContext)
    {
      // NOT TAILCALL - PUSH NEW STACK FRAME

      ++ interp->callStackTop;

      if (interp->callStackTop >= interp->callStackSize)
      {
        GURU(CALL_STACK_OVERFLOW);
      }

      current = InterpStackFrame(interp);

      InterpIncrContextCounter(interp);
    }
  }
  
  // Set first instruction in the new frame
  current->instruction = MemItemFirst(interp->mem, code);
}

void InterpPopStackFrame(VInterp* interp)
{
  //printf("POP STACK FRAME\n");

  if (interp->callStackTop < 0)
  {
    GURU(CALL_STACK_IS_EMPTY);
  }

  -- interp->callStackTop;

  InterpDecrContextCounter(interp);
}

// -------------------------------------------------------------
// Global vars
// -------------------------------------------------------------

void InterpSetGlobalVar(VInterp* interp, int index, VItem* item)
{
  if (index < interp->globalVarsSize)
  {
    (interp->globalVars)[index] = *item;
  }
  else
  {
    GURU(GLOBALVARS_OVERFLOW);
  }
}

#define InterpGetGlobalVar(interp, index) (& (((interp)->globalVars)[index]))

// -------------------------------------------------------------
// Eval
// -------------------------------------------------------------

int InterpEvalSlice(register VInterp* interp, register int sliceSize);

void InterpEval(VInterp* interp, VItem* code)
{
  InterpPushStackFrame(interp, code);
  InterpEvalSlice(interp, 0);
  // TODO: GC
}

// Evaluate a slice of code. 
// sliceSize specifies the number of instructions to execute.
// sliceSize 0 means eval as one slice until program ends.
// Returns done flag (TRUE = done, FALSE = not done).
int InterpEvalSlice(VInterp* interp, int sliceSize)
{
  VStackFrame* current;
  VItem*       instruction;
  VItem*       symbolValue;
  int          primFun;
  int          sliceCounter = 0;

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

    // Get instruction pointer
    current = InterpStackFrame(interp);
    instruction = current->instruction;

    // Evaluate current instruction.
    if (NULL != instruction)
    {
      // Advance instruction for next loop
      current->instruction = MemItemNext(interp->mem, current->instruction);

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
        InterpPush(interp, instruction);
      }
      else
      if (IsTypeSymbol(instruction))
      {
        VItem* value = InterpGetGlobalVar(interp, instruction->intNum);
        //if (!IsTypeNone(value))
        if (IsTypeFun(value))
        {
          InterpPushStackFrame(interp, value);
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
    else // NULL == instruction
    {
      InterpPopStackFrame(interp);

      // Exit if this was the last stackframe.
      if (interp->callStackTop < 0)
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
