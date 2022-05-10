/*
File: interp.h
Author: Mikael Kindborg (mikael@kindborg.com)

Interpreter data structures and functions.
*/

// -------------------------------------------------------------
// Data types and structs
// -------------------------------------------------------------

typedef struct __VStackFrame VCallStackFrame;

struct __VCallStackFrame
{
  VItem* instruction;  // Current instruction
  VInt   flags;
}


typedef struct __VStackFrame VRegStackFrame;

// Stack entry that holds local vars
struct __VRegStackFrame
{
  VItem  localVars[4]; // Space for 4 local vars
};

// Mind control for hackers

// mindfulness = ta kontroll över sitt tänkande, sina tankar

typedef struct __VInterp
{
  int              run; 

  int              globalVarsSize;
  VItem*           globalVars;
  VItem*           globalVarsEnd;

  int              dataStackSize;
  VItem*           dataStack;
  VItem*           dataStackEnd
  VItem*           dataStackTop;   // Top of data stack

  int              callStackSize;
  VCallStackFrame* callStack;
  VCallStackFrame* callStackEnd;
  VCallStackFrame* callStackTop;   // Current stackframe

  int              regStackSize;
  VRegStackFrame*  regStack;
  VRegStackFrame*  regStackEnd;
  VRegStackFrame*  refStackTop;    // Top of local var stack

  VMem*            mem;            // Item memory
}
VInterp;

// Prim fun lookup function
VPrimFunPtr LookupPrimFunPtr(int index);

#define InterpStackFrame(interp) ( & ((interp)->callStack[(interp)->callStackTop]) )
#define InterpStackFrameAt(interp, index) ( & ((interp)->callStack[index]) )

// -------------------------------------------------------------
// Interp
// -------------------------------------------------------------

VInterp* InterpNewWithSize(
  int globalVarsSize, int dataStackSize,
  int callStackSize, int regStackSize, int memSize)
{
  int globalVarsByteSize = globalVarsSize * sizeof(VItem);
  int dataStackByteSize = dataStackSize * sizeof(VItem);
  int callStackByteSize = callStackSize * sizeof(VCallStackFrame);
  int regStackByteSize = regStackSize * sizeof(VRegStackFrame);

  VInterp* interp = SysAlloc(
    sizeof(VInterp) + globalVarsByteSize + 
    dataStackByteSize + callStackByteSize + 
    regStackByteSize + MemByteSize(memSize));

  interp->globalVars = ((void*)interp) + sizeof(VInterp);
  interp->globalVarsSize = globalVarsSize;
  interp->globalVarsEnd = interp->globalVars + interp->globalVarsSize - 1;

  interp->dataStack = ((void*)interp->globalVarsEnd) + 1;
  interp->dataStackSize = dataStackSize;
  interp->dataStackEnd = interp->dataStack + interp->dataStackSize - 1;
  interp->dataStackTop = interp->dataStack - 1;

  interp->callStack = ((void*)interp->dataStackEnd) + 1;
  interp->callStackSize = callStackSize;
  interp->callStackEnd = interp->callStack + interp->callStackSize - 1;
  interp->callStackTop = interp->callStack - 1;

  interp->regStack = ((void*)interp->callStackEnd) + 1;
  interp->regStackSize = regStackSize;
  interp->regStackEnd = interp->regStack + interp->regStackSize - 1;
  interp->regStackTop = interp->regStack - 1;

  interp->mem = ((void*)interp->regStack) + 1;
  MemInit(interp->mem, memSize);

  GSymbolTableInit();

  return interp;
}

VInterp* InterpNew()
{
  return InterpNewWithSize(
    100, // globalVarsSize,
    100, // dataStackSize, 
    100, // callStackSize, 
    100, // regStackSize
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

void InterpGC(VInterp* interp)
{
  VItem* p;

  // Mark data stack
  for (p = interp->dataStack; p <= interp->dataStackTop; ++ p)
  {
    VItem* item = &(stack[i]);
    if (!IsTypeAtomic(p))
    {
      //PrintLine("STACK MARK:");
      //MemPrintItem(interp->mem, p); PrintNewLine();
      
      // We are screwed if item is a TypeBufferPtr
      MemMark(interp->mem, MemItemFirst(interp->mem, p));
    }
  }

  // Mark global vars
  VItem* lastEntry = interp->globalVars + globalVarsSize - 1;

  for (p = interp->globalVars; p <= lastEntry interp->dataStackTop; ++ p)
  {
    if (!IsTypeAtomic(p))
    {
      //PrintLine("GLOBALVAR MARK:");
      //MemPrintItem(interp->mem, item); PrintNewLine();

      // We are screwed if item is a TypeBufferPtr
      MemMark(interp->mem, MemItemFirst(interp->mem, p));
    }
  }

  // TODO: MemMark(interp->regStack); // Mark localvars

  MemSweep(interp->mem);
  
#ifdef TRACK_MEMORY_USAGE
  MemPrintAllocCounter(interp->mem);
#endif
}

// -------------------------------------------------------------
// Data stack
// -------------------------------------------------------------

void InterpStackPush(VInterp* interp, VItem *item)
{
  ++ interp->dataStackTop;
  if (interp->dataStackTop > interp->dataStackEnd)
  {
    GURU(DATA_STACK_OVERFLOW);
  }
  
  // Copy item
  interp->dataStack[interp->dataStackTop] = *item;
}

VItem* InterpStackPop(VInterp* interp)
{
  if (interp->dataStackTop < interp->dataStack)
  {
    GURU(DATA_STACK_IS_EMPTY);
  }

  return interp->dataStackTop --;
}

#define InterpStackAt(interp, offsetFromTop) \
  ( (interp)->dataStackTop - (offsetFromTop) )

#define InterpStackTop(interp) \
  ( (interp)->dataStackTop )

// -------------------------------------------------------------
// Call stack
// -------------------------------------------------------------

void InterpPushFirstStackFrame(VInterp* interp, VItem* code)
{
  // Set first stackframe
  interp->callStackTop = 0;
  VStackFrame* current = InterpStackFrame(interp);

  // Set first instruction in the frame
  current->instruction = MemItemFirst(interp->mem, code);
}

void InterpPushEvalStackFrame(VInterp* interp, VItem* code)
{
  // The current stackframe is the parent for the new stackframe
  VStackFrame* parent = InterpStackFrame(interp);

  // Assume tailcall
  VStackFrame* current = parent;

  // Check tailcall (are there any instructions left?)
  if (parent->instruction)
  {
    // NON-TAILCALL - PUSH NEW STACK FRAME

    ++ interp->callStackTop;

    if (interp->callStackTop >= interp->callStackSize)
    {
      GURU(CALL_STACK_OVERFLOW);
    }

    current = InterpStackFrame(interp);

    // Eval uses the parent environment
    current->context = parent;
  }

  // Set first instruction in the new frame
  current->instruction = MemItemFirst(interp->mem, code);
}

void InterpPushFunCallStackFrame(VInterp* interp, VItem* code)
{
  // The current stackframe is the parent for the new stackframe
  VStackFrame* parent = InterpStackFrame(interp);

  // Assume tailcall
  VStackFrame* current = parent;

  // Check tailcall (are there any instructions left?)
  if (parent->instruction)
  {
    // NON-TAILCALL - PUSH NEW STACK FRAME

    ++ interp->callStackTop;

    if (interp->callStackTop >= interp->callStackSize)
    {
      GURU(CALL_STACK_OVERFLOW);
    }

    current = InterpStackFrame(interp);
  }

  // Functions have their own local environment
  current->context = current;

  // Set first instruction in the new frame
  current->instruction = MemItemFirst(interp->mem, code);
}

void InterpPopStackFrame(VInterp* interp)
{
  if (interp->callStackTop < 0)
  {
    GURU(CALL_STACK_IS_EMPTY);
  }

  -- interp->callStackTop;
}

// -------------------------------------------------------------
// Local vars
// -------------------------------------------------------------

void InterpSetLocalVar(VInterp* interp, int index, VItem* item)
{
  VStackFrame* frame = InterpStackFrame(interp);
  frame->context->localVars[index] = *item;
}

#define InterpGetLocalVar(interp, index) \
  ( & (InterpStackFrame(interp)->context->localVars[index]) )

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

int InterpEvalSlice(VInterp* interp, int sliceSize);

void InterpEval(VInterp* interp, VItem* code)
{
  InterpPushFirstStackFrame(interp, code);
  InterpEvalSlice(interp, 0);
  InterpGC(interp);
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
        InterpStackPush(interp, instruction);
      }
      else
      // TODO: Don't push unbound symbols?
      if (IsTypeSymbol(instruction))
      {
        VItem* value = InterpGetGlobalVar(interp, instruction->intNum);
        if (IsTypeFun(value))
        {
          // Call function
          InterpPushFunCallStackFrame(interp, value);
        }
        else
        if (!IsTypeNone(value))
        {
          // Push value
          InterpStackPush(interp, value);
        }
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
