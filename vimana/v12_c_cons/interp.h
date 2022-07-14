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
stackframe  12 bytes (2+2+4+4) // 2 local vars

callstack   10 x 12 =  120 bytes
datastack   10 x 4  =   40 bytes
globalvars  20 x 4  =   80 bytes
items      200 x 4  =  800 bytes
+ sizeof(VInterp)       20 bytes
+ sizeof(VListMemory)    8 bytes
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
  VItem*       list;         // List that is evaluated
  VItem*       instruction;  // Current instruction
  VStackFrame* context;      // Stack frame that holds local vars
  VItem        localVars[4]; // Space for 4 local vars
};

/*
Alternatives for dynamic number of local vars:

struct __VStackFrame
{
  VItem*       list;         // List that is evaluated
  VItem*       instruction;  // Current instruction
  VStackFrame* context;      // Stack frame that holds local vars
  VItem*       localVars;    // List of localvars
};

struct __VStackFrame
{
  VItem*       list;         // List that is evaluated
  VItem*       instruction;  // Current instruction
  VStackFrame* parent;       // Parent stack frame
  VStackFrame* context;      // Stack frame that holds local vars
  int          numlocals;    // number of localvars that follow the stackframe
};
*/

// Mind control for hackers
// mindfulness = ta kontroll över sitt tänkande, sina tankar

typedef struct __VInterp
{
  int             run;                 // Run flag

  VItem*          globalVarTable;      // Global items
  int             globalVarTableSize;  // Max number of global vars

  VItem*          dataStack;           // Data stack items
  int             dataStackSize;       // Max number of item on the stack
  int             dataStackTop;        // Top of datastack

  VStackFrame*    callStack;           // Callstack frames
  int             callStackSize;       // Max number of frames
  int             callStackTop;        // Current stackframe

  VListMemory*    listMemory;          // Lisp-style list memory
}
VInterp;

#define InterpListMem(interp) ((interp)->listMemory)

// Seems to be slower?
//#define InterpGetStackFrame(interp) ( ((interp)->callStack) + ((interp)->callStackTop) )
#define InterpGetStackFrame(interp) ( & ((interp)->callStack[(interp)->callStackTop]) )
// UNUSED
//#define InterpGetStackFrameAt(interp, index) ( & ((interp)->callStack[index]) )

// -------------------------------------------------------------
// Interp
// -------------------------------------------------------------

void PrintAlignedPtr(char* text, void* ptr)
{
  unsigned long addr = (unsigned long)ptr;
  unsigned long offset = addr % 8;

  printf("PTR %s: %lu\n", text, addr);
  printf("ALN %s: %lu\n", text, offset);
}

int InterpByteSize(
  int sizeGlobalVarTable, int sizeDataStack, 
  int sizeCallStack,      int sizeListMemory)
{
  int byteSizeInterpStruct = sizeof(VInterp);
  int byteSizeGlobalVarTable = sizeGlobalVarTable * sizeof(VItem);
  int byteSizeDataStack = sizeDataStack * sizeof(VItem);
  int byteSizeCallStack = sizeCallStack * sizeof(VStackFrame);
  int byteSizeListMemory = ListMemByteSize(sizeListMemory);

  int byteSizeInterp = 
    byteSizeInterpStruct +
    byteSizeGlobalVarTable +
    byteSizeDataStack +
    byteSizeCallStack +
    byteSizeListMemory;

  return byteSizeInterp;
}

// TODO: Check aligntment of pointers
void InterpInit(
  VInterp* interp,   
  int sizeGlobalVarTable, int sizeDataStack, 
  int sizeCallStack,      int sizeListMemory)
{
  int byteSizeInterpStruct = sizeof(VInterp);
  int byteSizeGlobalVarTable = sizeGlobalVarTable * sizeof(VItem);
  int byteSizeDataStack = sizeDataStack * sizeof(VItem);
  int byteSizeCallStack = sizeCallStack * sizeof(VStackFrame);
  int byteSizeListMemory = ListMemByteSize(sizeListMemory);

  interp->globalVarTable = PtrOffset(interp, byteSizeInterpStruct);
  interp->globalVarTableSize = sizeGlobalVarTable;
  
  interp->dataStack = PtrOffset(interp->globalVarTable, byteSizeGlobalVarTable);
  interp->dataStackSize = sizeDataStack;
  interp->dataStackTop = -1;

  interp->callStack = PtrOffset(interp->dataStack, byteSizeDataStack);
  interp->callStackSize = sizeCallStack;
  interp->callStackTop = -1;

  interp->listMemory = PtrOffset(interp->callStack, byteSizeListMemory);
  ListMemInit(interp->listMemory, sizeListMemory);

  #ifdef DEBUG
    PrintAlignedPtr("INTERP", interp);
    PrintAlignedPtr("GLOBVR", interp->globalVarTable);
    PrintAlignedPtr("DATAST", interp->dataStack);
    PrintAlignedPtr("CALLST", interp->callStack);
    PrintAlignedPtr("MEMORY", interp->listMemory);
    PrintLine("------------------------------------------------");
  #endif
}

void InterpFree(VInterp* interp)
{
  ListMemSweep(InterpListMem(interp));

  #ifdef TRACK_MEMORY_USAGE
    ListMemPrintAllocCounter(InterpListMem(interp));
  #endif
}

// -------------------------------------------------------------
// Item access
// -------------------------------------------------------------

#define GetFirst(item, interp) ListMemGetFirst(InterpListMem(interp), item)
#define GetNext(item, interp) ListMemGetNext(InterpListMem(interp), item)

#define SetFirst(item, first, interp) ListMemSetFirst(InterpListMem(interp), item, first)
#define SetNext(item, next, interp) ListMemSetNext(InterpListMem(interp), item, next)

#define AllocItem(interp) ListMemAlloc(InterpListMem(interp))
#define AllocHandle(data, type, interp) ListMemAllocHandle(InterpListMem(interp), data, type)
#define GetHandlePtr(item, interp) ListMemGetHandlePtr(InterpListMem(interp), item)

// -------------------------------------------------------------
// Garbage collection
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
      ListMemMark(InterpListMem(interp), GetFirst(item, interp));
    }
  }

  // Mark global vars
  VItem* table = interp->globalVarTable;
  for (int i = 0; i < interp->globalVarTableSize; ++ i)
  {
    VItem* item = & (table[i]);
    if (!IsTypeAtomic(item))
    {
      ListMemMark(InterpListMem(interp), GetFirst(item, interp));
    }
  }

  // TODO: ListMemMark(callstack); // Walk from top and mark localvars

  ListMemSweep(InterpListMem(interp));

  #ifdef TRACK_MEMORY_USAGE
    ListMemPrintAllocCounter(InterpListMem(interp));
  #endif
}

// -------------------------------------------------------------
// Data stack
// -------------------------------------------------------------

// Copies the item to the stack
void InterpStackPush(VInterp* interp, VItem* item)
{
  ++ interp->dataStackTop;

  if (interp->dataStackTop >= interp->dataStackSize)
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

void InterpPushFirstStackFrame(VInterp* interp, VItem* list)
{
  // Set first stackframe
  interp->callStackTop = 0;
  VStackFrame* current = InterpGetStackFrame(interp);
  current->context = current;

  // Set first instruction in the frame
  current->instruction = GetFirst(list, interp);
}

void InterpPushStackFrame(VInterp* interp, VItem* list)
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

    if (interp->callStackTop >= interp->callStackSize)
    {
      GURU_MEDITATION(CALL_STACK_OVERFLOW);
    }

    current = InterpGetStackFrame(interp);

    // Access the local vars of the parent context until new scope is introduced
    current->context = parent->context;
  }

  // Set first instruction in the new frame
  current->instruction = GetFirst(list, interp);
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

  // Set context to this stackframe when a local variable is introduced.
  // This creates a new "scope".
  // Note: Functions cannot access variables in the parent scope, 
  // only read them. This is by design.
  frame->context = frame;

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
  if (index < interp->globalVarTableSize)
  {
    // Copy item
    (interp->globalVarTable)[index] = *item;
  }
  else
  {
    GURU_MEDITATION(GLOBAL_VAR_TABLE_OVERFLOW);
  }
}

#define InterpGetGlobalVar(interp, index) \
  (& (((interp)->globalVarTable)[index]))

// -------------------------------------------------------------
// Eval
// -------------------------------------------------------------

int InterpEvalSlice(VInterp* interp, int sliceSize);

void InterpEval(VInterp* interp, VItem* list)
{
  InterpPushFirstStackFrame(interp, list);
  InterpEvalSlice(interp, 0);
  InterpGC(interp);
}

// Evaluate a slice of the program list. 
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

  #ifdef DEBUG
    int  callstackMax = 0;
    long interpLoops = 0;
  #endif

  interp->run = TRUE;

  while (interp->run)
  {
    #ifdef DEBUG
      if (interp->callStackTop > callstackMax) callstackMax = interp->callStackTop;
      ++ interpLoops;
    #endif

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
      // Advance instruction for the *NEXT* loop
      current->instruction = GetNext(instruction, interp);

      if (IsTypePrimFun(instruction))
      {
        VPrimFunPtr fun = ItemGetPrimFun(instruction);
        fun(interp);
      }
      else
      if (IsTypePushable(instruction))
      {
        InterpStackPush(interp, instruction);
      }
      else
      if (IsTypeSymbol(instruction))
      {
        VItem* value = InterpGetGlobalVar(interp, ItemGetSymbol(instruction));
        if (IsTypeFun(value))
        {
          // Call function
          InterpPushStackFrame(interp, value);
        }
        else
        if (!IsTypeNone(value))
        {
          // Push value
          InterpStackPush(interp, value);
        }
      }
    }
    else // (NULL == instruction)
    {
      InterpPopStackFrame(interp);

      interp->run = interp->callStackTop > -1;
/*
      // Exit if this was the last stackframe.
      if (interp->callStackTop < 0)
      {
        interp->run = FALSE;
        goto Exit; // Exit loop
      }
*/
    }
  }
  // while

Exit:

  #ifdef DEBUG
    PrintLine("EXIT INTERP LOOP");
    Print("CALLSTACK MAX: "); PrintIntNum(callstackMax); PrintNewLine();
    Print("INTERP LOOPS: "); PrintIntNum(interpLoops); PrintNewLine();
  #endif

  return ! interp->run;
}
