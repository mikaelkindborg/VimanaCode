/*
File: interp.h
Author: Mikael Kindborg (mikael@kindborg.com)

Interpreter data structures and functions.
*/

// -------------------------------------------------------------
// Data types and structs
// -------------------------------------------------------------

/*
Arduino Uno example memory layout, fit in 2K

item         4 bytes (2+2)
stackframe  12 bytes (2+2+4+4)

callstack   10 x 12 =  120 bytes
datastack   10 x 4  =   40 bytes
globalvars  20 x 4  =   80 bytes
items      200 x 4  =  800 bytes
+ sizeof(VInterp)       20 bytes
+ sizeof(VMem)           8 bytes
                      ----------
Core memory           1068 bytes

primstrings 50 x 5  =  250 bytes - put in PROGMEM
globalstr   20 x 6  =  120 bytes
                      ----------
String memory          370 bytes

                      ----------
Total                 1438 bytes
*/

typedef struct __VStackFrame VStackFrame;

struct __VStackFrame
{
  VItem*       instruction;  // Current instruction
  VStackFrame* context;      // Stack frame that holds local vars
  VItem        localVars[4]; // Space for 4 local vars
};

// Mind control for hackers

// mindfulness = ta kontroll över sitt tänkande, sina tankar

typedef struct __VInterp
{
  int          run; 

  int          numGlobalVars;
  VItem*       globalVars;

  int          numDataStackItems;
  int          dataStackTop;
  VItem*       dataStack;

  int          numCallStackFrames;
  int          callStackTop;        // Current stackframe
  VStackFrame* callStack;

  VMem*        mem;                 // Item memory

}
VInterp;

// Prim fun lookup function
VPrimFunPtr LookupPrimFunPtr(int index);

#define InterpMem(interp) ((interp)->mem)

// Seems to be slower?
//#define InterpGetStackFrame(interp) ( ((interp)->callStack) + ((interp)->callStackTop) )
#define InterpGetStackFrame(interp) ( & ((interp)->callStack[(interp)->callStackTop]) )
// UNUSED
//#define InterpGetStackFrameAt(interp, index) ( & ((interp)->callStack[index]) )

// -------------------------------------------------------------
// Interp
// -------------------------------------------------------------

VInterp* InterpNewWithSize(
  int numGlobalVars, int numDataStackItems,
  int numCallStackFrames, int numItems)
{
  int globalVarsByteSize = numGlobalVars * sizeof(VItem);
  int dataStackByteSize  = numDataStackItems * sizeof(VItem);
  int callStackByteSize  = numCallStackFrames * sizeof(VStackFrame);
  int memByteSize        = MemGetByteSize(numItems);

  VInterp* interp = SysAlloc(
    sizeof(VInterp) + globalVarsByteSize + 
    dataStackByteSize + callStackByteSize + 
    memByteSize);

  interp->globalVars = (VItem*) BytePtrOffset(interp, sizeof(VInterp));
  interp->numGlobalVars = numGlobalVars;

  interp->dataStack = (VItem*) BytePtrOffset(interp->globalVars, globalVarsByteSize);
  interp->numDataStackItems = numDataStackItems;
  interp->dataStackTop = -1;

  interp->callStack = (VStackFrame*) BytePtrOffset(interp->dataStack, dataStackByteSize);
  interp->numCallStackFrames = numCallStackFrames;
  interp->callStackTop = 0;

  interp->mem = (VMem*) BytePtrOffset(interp->callStack, callStackByteSize);
  MemInit(InterpMem(interp), numItems);

  return interp;
}

VInterp* InterpNew()
{
  return InterpNewWithSize(
    100, // numGlobalVars,
    100, // numDataStackItems, 
    100, // numCallStackFrames
    1000 // numItems
  );
}

void InterpFree(VInterp* interp)
{
  MemSweep(InterpMem(interp));

#ifdef TRACK_MEMORY_USAGE
  MemPrintAllocCounter(InterpMem(interp));
#endif

  SysFree(interp);
}

// -------------------------------------------------------------
// Item access
// -------------------------------------------------------------

#define InterpGetFirst(interp, list) MemGetFirst(InterpMem(interp), list)
#define InterpGetNext(interp, item) MemGetNext(InterpMem(interp), item)
#define InterpSetFirst(interp, list, first) MemSetFirst(InterpMem(interp), list, first)
#define InterpSetNext(interp, item, next) MemSetNext(InterpMem(interp), item, next)

#define InterpAllocItem(interp) MemAllocItem(InterpMem(interp))
#define InterpAllocBuffer(interp, data) MemAllocBuffer(InterpMem(interp), data)
#define InterpGetBufferPtr(interp, item) MemGetBufferPtr(InterpMem(interp), item)

// -------------------------------------------------------------
// GC
// -------------------------------------------------------------

void InterpGC(VInterp* interp)
{
  // Mark data stack
  VItem* stack = interp->dataStack;
  for (int i = 0; i <= interp->dataStackTop; ++ i)
  {
    VItem* item = &(stack[i]);
    if (!IsTypeAtomic(item))
    {
      //PrintLine("STACK MARK:");
      //MemPrintItem(interp->mem, item); PrintNewLine();
      
      // We are screwed if item is a TypeBufferPtr
      MemMark(InterpMem(interp), InterpGetFirst(interp, item));
    }
  }

  // Mark global vars
  VItem* globalVars = interp->globalVars;
  for (int i = 0; i < interp->numGlobalVars; ++ i)
  {
    VItem* item = &(globalVars[i]);
    if (!IsTypeAtomic(item))
    {
      //PrintLine("GLOBALVAR MARK:");
      //MemPrintItem(interp->mem, item); PrintNewLine();

      // We are screwed if item is a TypeBufferPtr
      MemMark(InterpMem(interp), InterpGetFirst(interp, item));
    }
  }

  //InterpMark(callstack); // Walk from top and mark localvars

  MemSweep(InterpMem(interp));

#ifdef TRACK_MEMORY_USAGE
  MemPrintAllocCounter(InterpMem(interp));
#endif
}

// -------------------------------------------------------------
// Data stack
// -------------------------------------------------------------

// Copies item
void InterpStackPush(VInterp* interp, VItem* item)
{
  ++ interp->dataStackTop;

  if (interp->dataStackTop >= interp->numDataStackItems)
  {
    GURU_MEDITATION(DATA_STACK_OVERFLOW);
  }
  
  // Copy item
  interp->dataStack[interp->dataStackTop] = *item;
}

VItem* InterpStackPop(VInterp* interp)
{
  if (interp->dataStackTop < 0)
  {
    GURU_MEDITATION(DATA_STACK_IS_EMPTY);
  }

  return & (interp->dataStack[interp->dataStackTop --] );
}

/* Not faster
#define InterpStackPop(interp) \
  ( ((interp)->dataStackTop < 0) ? \
    ( GURU_MEDITATION(DATA_STACK_IS_EMPTY), NULL ) : \
    ( (VItem*) & ((interp)->dataStack[(interp)->dataStackTop --]) ) )
*/  

#define InterpStackAt(interp, offsetFromTop) \
  ( & ((interp)->dataStack[(interp)->dataStackTop - (offsetFromTop)]) )

#define InterpStackTop(interp) \
  ( & ((interp)->dataStack[(interp)->dataStackTop]) )

// -------------------------------------------------------------
// Call stack
// -------------------------------------------------------------

void InterpPushFirstStackFrame(VInterp* interp, VItem* code)
{
  // Set first stackframe
  interp->callStackTop = 0;
  VStackFrame* current = InterpGetStackFrame(interp);
  current->context = current;

  // Set first instruction in the frame
  current->instruction = InterpGetFirst(interp, code);
}

void InterpPushEvalStackFrame(VInterp* interp, VItem* code)
{
  // The current stackframe is the parent for the new stackframe
  VStackFrame* parent = InterpGetStackFrame(interp);

  // Assume tailcall
  VStackFrame* current = parent;

  // Check tailcall (are there any instructions left?)
  if (parent->instruction)
  {
    // NON-TAILCALL - PUSH NEW STACK FRAME

    ++ interp->callStackTop;

    if (interp->callStackTop >= interp->numCallStackFrames)
    {
      GURU_MEDITATION(CALL_STACK_OVERFLOW);
    }

    current = InterpGetStackFrame(interp);

    // Eval uses the parent environment
    current->context = parent;
  }

  // Set first instruction in the new frame
  current->instruction = InterpGetFirst(interp, code);
}

void InterpPushFunCallStackFrame(VInterp* interp, VItem* code)
{
  // The current stackframe is the parent for the new stackframe
  VStackFrame* parent = InterpGetStackFrame(interp);

  // Assume tailcall
  VStackFrame* current = parent;

  // Check tailcall (are there any instructions left?)
  if (parent->instruction)
  {
    // NON-TAILCALL - PUSH NEW STACK FRAME

    ++ interp->callStackTop;

    if (interp->callStackTop >= interp->numCallStackFrames)
    {
      GURU_MEDITATION(CALL_STACK_OVERFLOW);
    }

    current = InterpGetStackFrame(interp);
  }

  // Functions have their own local environment
  current->context = current;

  // Set first instruction in the new frame
  current->instruction = InterpGetFirst(interp, code);
}

void InterpPopStackFrame(VInterp* interp)
{
  if (interp->callStackTop < 0)
  {
    GURU_MEDITATION(CALL_STACK_IS_EMPTY);
  }

  -- interp->callStackTop;
}

// -------------------------------------------------------------
// Local vars
// -------------------------------------------------------------

// Copies item
void InterpSetLocalVar(VInterp* interp, int index, VItem* item)
{
  VStackFrame* frame = InterpGetStackFrame(interp);

  // Copy item
  frame->context->localVars[index] = *item;
}

#define InterpGetLocalVar(interp, index) \
  ( & (InterpGetStackFrame(interp)->context->localVars[index]) )

// -------------------------------------------------------------
// Global vars
// -------------------------------------------------------------

// Copies item
void InterpSetGlobalVar(VInterp* interp, int index, VItem* item)
{
  if (index < interp->numGlobalVars)
  {
    // Copy item
    (interp->globalVars)[index] = *item;
  }
  else
  {
    GURU_MEDITATION(GLOBALVARS_OVERFLOW);
  }
}

#define InterpGetGlobalVar(interp, index) \
  (& (((interp)->globalVars)[index]))

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

  //int  callstackMax = 0;
  //long callstackLoops = 0;

  interp->run = TRUE;

  while (interp->run)
  {
    //if (interp->callStackTop > callstackMax) callstackMax = interp->callStackTop;
    //++ callstackLoops;

    // Count slices if a sliceSize is specified.
    if (sliceSize)
    {
      if (sliceSize > sliceCounter)
        ++ sliceCounter;
      else
        goto Exit; // Exit loop
    }

    // Get instruction pointer
    current = InterpGetStackFrame(interp);
    instruction = current->instruction;

    // Evaluate current instruction.
    if (NULL != instruction)
    {
      // Advance instruction for next loop
      current->instruction = InterpGetNext(interp, current->instruction);

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
        else
        {
          // TODO: Allow unbound symbols?
          // Push unbound symbol
          //InterpStackPush(interp, instruction);
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
  //PrintLine("EXIT INTERP LOOP");
  //Print("CALLSTACK MAX: "); PrintIntNum(callstackMax); PrintNewLine();
  //Print("INTERP LOOPS: "); PrintIntNum(callstackLoops); PrintNewLine();
  return ! interp->run;
}
