/*
File: interp.h
Author: Mikael Kindborg (mikael@kindborg.com)

Interpreter data structures and functions.
*/

// -------------------------------------------------------------
// Data types and structs
// -------------------------------------------------------------

enum CALL_TYPE
{
  CALL_TYPE_EVAL,
  CALL_TYPE_FUN
};

typedef struct __VStackFrame
{
  VItem*                instruction;
  struct __VStackFrame* context;
  VItem                 localVars[4];
}
VStackFrame;

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

  int          callStackSize;
  int          callStackTop; // Current context
  VStackFrame* callStack;
}
VInterp;

typedef struct __VInterp VInterp;
typedef void (*VPrimFunPtr) (VInterp*);

VPrimFunPtr LookupPrimFunPtr(int index);

#define InterpStackFrame(interp) ( ((interp)->callStack) + ((interp)->callStackTop) )
// TODO #define InterpStackFrame(interp) ( & ((interp)->callStack[(interp)->callStackTop] )

// -------------------------------------------------------------
// Interp
// -------------------------------------------------------------

VInterp* InterpNewWithSize(
  int globalVarsSize, int dataStackSize,
  int callStackSize, int memSize)
{
  int globalVarsByteSize = globalVarsSize * sizeof(VItem);
  int dataStackByteSize = dataStackSize * sizeof(VItem);
  int callStackByteSize = callStackSize * sizeof(VStackFrame);

  VInterp* interp = SysAlloc(
    sizeof(VInterp) + globalVarsByteSize + 
    dataStackByteSize + callStackByteSize);

  interp->globalVars = (void*)interp + sizeof(VInterp);
  interp->globalVarsSize = globalVarsSize;

  interp->dataStack = (void*)interp->globalVars + globalVarsByteSize;
  interp->dataStackSize = dataStackSize;
  interp->dataStackTop = -1;

  interp->callStack = (void*)interp->dataStack + dataStackByteSize;
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
}

/* Not faster
#define InterpPop(interp) \
  ( ((interp)->dataStackTop < 0) ? \
    ( GURU(DATA_STACK_IS_EMPTY), NULL ) : \
    ( (VItem*) & ((interp)->dataStack[(interp)->dataStackTop --]) ) )
*/  

#define InterpStackAt(interp, offsetFromTop) \
  ( & ((interp)->dataStack[(interp)->dataStackTop - (offsetFromTop)]) )

#define InterpStackTop(interp) \
  ( & ((interp)->dataStack[(interp)->dataStackTop]) )

// -------------------------------------------------------------
// Call stack
// -------------------------------------------------------------

void InterpPushStackFrameImpl(VInterp* interp, VItem* code, VSmallInt callType)
{
  VStackFrame* current;

  if (interp->callStackTop < 0)
  {
    // EMPTY CALLSTACK - CREATE ROOT FRAME

    ++ interp->callStackTop;
    current = InterpStackFrame(interp);
    current->context = current;
  }
  else
  {
    VStackFrame* parent = InterpStackFrame(interp);

    // Check tailcall (are there any instructions left?)
    void* instructionsLeft = parent->instruction;
    if (instructionsLeft)
    {
      // NON-TAILCALL - PUSH NEW STACK FRAME

      ++ interp->callStackTop;

      if (interp->callStackTop >= interp->callStackSize)
      {
        GURU(CALL_STACK_OVERFLOW);
      }

      current = InterpStackFrame(interp);

      if (CALL_TYPE_FUN == callType) 
      {
        // Functions have their own local environment
        current->context = current;
      }
      else
      {
        // Eval uses the parent environment
        current->context = parent;
      }
    }
    else
    {
      // TAILCALL

      current = parent;

      if (CALL_TYPE_FUN == callType) 
      {
        // Functions have their own local environment
        current->context = current;
      }
    }
  }
  
  // Set first instruction in the frame
  current->instruction = MemItemFirst(interp->mem, code);
}

#define InterpPushStackFrame(interp, code) \
  InterpPushStackFrameImpl(interp, code, CALL_TYPE_EVAL)

#define InterpPushStackFrameFun(interp, code) \
  InterpPushStackFrameImpl(interp, code, CALL_TYPE_FUN)

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
        if (IsTypeFun(value))
        {
          // Call function
          InterpPushStackFrameFun(interp, value);
        }
        else
        {
          // Push value
          InterpPush(interp, value);
        }
      }
      /*
      else
      {
        printf("Instruction type: %i\n", ItemType(instruction)); 
        GURU(INTERP_UNEXPECTED_TYPE);
      }
      */
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
