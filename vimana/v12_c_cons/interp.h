/*
File: interp.h
Author: Mikael Kindborg (mikael@kindborg.com)

Interpreter data structures and functions.
*/

enum CALLTYPE
{
  CALLTYPE_FUN,
  CALLTYPE_EVAL,
  CALLTYPE_FUNX,
  CALLTYPE_EVALX
};

typedef struct __VContext
{
  struct __VContext* prev;           // Previous context in call stack
  struct __VContext* localVarCtx;    // Context that holds local vars
  VSmallInt          numVars;        // Number of local vars in this context
  VSmallInt          callType;       // Context type
  VItem*             code;           // Code list
  VItem*             instruction;    // Instruction pointer
  VItem              localVars[1];   // Memory area for local vars
}
VContext;

#define ContextSetLocalVar(context, index, itemPointer) \
  ((context)->localVars[index] = *(itemPointer))

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
  void*     callStackBounds;
  VContext* callStackTop;    // Current context
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
  interp->callStackBounds = (void*)interp->callStack + callStackByteSize;
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

void InterpPushContextImpl(VInterp* interp, VItem* code, int callType)
{
  VContext* currentContext = interp->callStackTop;

  if (NULL == currentContext)
  {
    // Set root context
    currentContext = interp->callStack;
    currentContext->prev = NULL;

    // The root context always has its own local vars
    currentContext->localVarCtx = currentContext;

    // Initialize number of local vars
    currentContext->numVars = 0;

    interp->callStackTop = currentContext;
  }
  else
  {
    // Check tailcall - push new context only if this is not the last instruction
    void* pushContext = interp->callStackTop->instruction;
    if (pushContext)
    {
      // NOT TAILCALL - PUSH NEW CONTEXT

      // Size of previous context
      size_t contextSize = sizeof(VContext) + (interp->callStackTop->numVars * sizeof(VItem));

      // Address of the new current context
      currentContext = (void*)interp->callStackTop + contextSize;

      // Bounds for callstack with room for 10 local vars in the new context
      void* bounds = (void*)currentContext + sizeof(VContext) + (10 * sizeof(VItem));
     
      if (bounds >= interp->callStackBounds)
      {
        GURU(CALL_STACK_OVERFLOW);
      }

      // Make new context top
      currentContext->prev = interp->callStackTop;
      interp->callStackTop = currentContext;

      // Set call type of new context
      currentContext->callType = callType;

      if (CALLTYPE_EVAL == callType)
      {
        // A new eval context uses the local vars of the previous context
        currentContext->localVarCtx = currentContext->prev->localVarCtx;
        currentContext->numVars = 0;
      }
    }
      
    // TAILCALL OR NEW CONTEXT

    // Always initialize local vars for function calls
    if (CALLTYPE_FUN == callType || CALLTYPE_FUNX == callType)
    {
      // A function call or special form has its own local vars
      currentContext->numVars = 0;
      currentContext->localVarCtx = currentContext;
    }

    // For tailcall CALLTYPE_EVAL we want to keep the current call type and context
    // if (CALLTYPE_EVAL == callType) DO_NOTHING();

    // For CALLTYPE_EVALX we want to find the closest function call context
    if (CALLTYPE_EVALX == callType)
    {
      VContext* context = currentContext;
      while (context)
      {
        // Have we found the function call context?
        if (CALLTYPE_FUN == context->callType)
        {
          currentContext->localVarCtx = context;
          break;
        }

        context = context->prev;
      }

      GURU(EVALX_NO_FUNCALL_CONTEXT_FOUND);
    }
  }

  // Set first instruction in new frame
  currentContext->code = code;
  currentContext->instruction = MemItemFirst(interp->mem, code);
}

#define InterpPushContext(interp, code) \
  InterpPushContextImpl(interp, code, CALLTYPE_EVAL)

#define InterpPushEvalXContext(interp, code) \
  InterpPushContextImpl(interp, code, CALLTYPE_EVALX)

#define InterpPushFunContext(interp, code) \
  InterpPushContextImpl(interp, code, CALLTYPE_FUN)

#define InterpPushFunXContext(interp, code) \
  InterpPushContextImpl(interp, code, CALLTYPE_FUNX)

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
