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
+ sizeof(VItemMemory)           8 bytes
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

/*
Alternatives for dynamic number local vars:

struct __VStackFrame
{
  VItem*       instruction;  // Current instruction
  VStackFrame* context;      // Stack frame that holds local vars
  VItem*       localVars;    // List of localvars
};

struct __VStackFrame
{
  VItem*       instruction;  // Current instruction
  VStackFrame* context;      // Stack frame that holds local vars
  int          numlocals;    // number of localvars that follow the stackframe
};

struct __VStackFrame
{
  VItem*       instruction;  // Current instruction
  VStackFrame* context;      // Stack frame that holds local vars
  int          hasLocals; 
  int          numlocals;    // number of localvars that follow the stackframe
};


*/

// Mind control for hackers
// mindfulness = ta kontroll över sitt tänkande, sina tankar

typedef struct __VInterp
{
  int             run;                 // Run flag

  VItem*          globalVarTable;      // Global items
  int             globalVarTableMax;   // Max number of global vars

  VPrimFun*       primFunTable;        // PrimFun function pointer table
  int             primFunTableMax;     // Max number of primfuns

  VItem*          dataStack;           // Data stack items
  int             dataStackMax;        // Max number of item on the stack
  int             dataStackTop;        // Top of datastack

  VStackFrame*    callStack;           // Callstack frames
  int             callStackMax;        // Max number of frames
  int             callStackTop;        // Current stackframe

  VItemMemory*    itemMemory;          // "Lisp" memory
  VSymbolTable*   symbolNames;         // Names of global vars
  VSymbolTable*   primFunNames;        // Names of primfuns
  VSymbolMemory*  symbolMemory;        // Immutable symbol memory
}
VInterp;

#define InterpMem(interp) ((interp)->itemMemory)
#define InterpSymbolMem(interp) ((interp)->symbolMemory)

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

// TODO: Check aligntment of pointers
VInterp* InterpNewWithSize(
  int sizeGlobalVarTable, int sizeDataStack, int sizeCallStack, 
  int sizePrimFunTable, int sizeItemMemory, int sizeSymbolMemory)
{
  // This block holds everything except strings, which are allocated in new blocks
  VInterp* interp = SysAlloc(
    sizeof(VInterp) + 
    sizeof(VItemMemory) + 
    (2 * sizeof(VSymbolMemory)) + 
    sizeGlobalVarTable +
    sizeDataStack +
    sizeCallStack +
    sizePrimFunTable +
    sizeItemMemory +
    sizeSymbolMemory);

  interp->globalVarTable = PtrOffset(interp, sizeof(VInterp));
  interp->numGlobalVars = sizeGlobalVars / sizeof(VItem);

  interp->primFunTable = PtrOffset(interp, sizeof(VInterp));
  interp->numGlobalVars = sizeGlobalVars / sizeof(VItem);
  
  VPrimFun*       primFunTable;        // PrimFun pointers
  int             primFunTableSize;    // Max number of primfuns

  interp->dataStack = PtrOffset(interp->globalVars, sizeGlobalVars);
  interp->numDataStackItems = sizeDataStack / sizeof(VItem);
  interp->dataStackTop = -1;

  interp->callStack = PtrOffset(interp->dataStack, sizeDataStack);
  interp->numCallStackFrames = sizeCallStack / sizeof(VStackFrame);
  interp->callStackTop = 0;

  interp->itemMemory = PtrOffset(interp->callStack, sizeCallStack);
  MemInit(interp->itemMemory, sizeItemMemory);

  interp->symbolMemory = PtrOffset(interp->callStack, sizeCallStack);
  MemInit(interp->symbolMemory, sizeSymbolMemory);

  #ifdef DEBUG
    PrintAlignedPtr("INTERP", interp);
    PrintAlignedPtr("GLOBVR", interp->globalVars);
    PrintAlignedPtr("DATAST", interp->dataStack);
    PrintAlignedPtr("CALLST", interp->callStack);
    PrintAlignedPtr("MEMORY", interp->itemMemory);
    PrintAlignedPtr("SYMMEM", interp->symbolMemory);
  #endif

  return interp;
}

VInterp* InterpNew()
{
  return InterpNewWithSize(
    100  * sizeof(VItem),       // sizeGlobalVars
    100  * sizeof(VItem),       // sizeDataStack
    100  * sizeof(VStackFrame), // sizeCallStack
    1000 * sizeof(VItem),       // sizeItemMemory 
    1024 * sizeof(char)         // sizeSymbolMemory
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

#define GetFirst(item, interp) MemGetFirst(InterpMem(interp), item)
#define GetNext(item, interp) MemGetNext(InterpMem(interp), item)

#define SetFirst(item, first, interp) MemSetFirst(InterpMem(interp), item, first)
#define SetNext(item, next, interp) MemSetNext(InterpMem(interp), item, next)

#define AllocItem(interp) MemAlloc(InterpMem(interp))
#define AllocHandle(data, type, interp) MemAllocHandle(InterpMem(interp), data, type)
#define GetHandlePtr(item, interp) MemGetHandlePtr(InterpMem(interp), item)

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
      MemMark(InterpMem(interp), GetFirst(item, interp));
    }
  }

  // Mark global vars
  VItem* globalVars = interp->globalVars;
  for (int i = 0; i < interp->numGlobalVars; ++ i)
  {
    VItem* item = &(globalVars[i]);
    if (!IsTypeAtomic(item))
    {
      MemMark(InterpMem(interp), GetFirst(item, interp));
    }
  }

  // TODO: MemMark(callstack); // Walk from top and mark localvars

  MemSweep(InterpMem(interp));

  #ifdef TRACK_MEMORY_USAGE
    MemPrintAllocCounter(InterpMem(interp));
  #endif
}

// -------------------------------------------------------------
// Data stack
// -------------------------------------------------------------

// Copies the item to the stack
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

    if (interp->callStackTop >= interp->numCallStackFrames)
    {
      GURU_MEDITATION(CALL_STACK_OVERFLOW);
    }

    current = InterpGetStackFrame(interp);

    // Access the local vars of the parent until new scope is introduced
    current->context = parent;
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
  // Note: Function cannot alter the variables in the parent scope, 
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
      // Advance instruction for next loop
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

  #ifdef DEBUG
    PrintLine("EXIT INTERP LOOP");
    Print("CALLSTACK MAX: "); PrintIntNum(callstackMax); PrintNewLine();
    Print("INTERP LOOPS: "); PrintIntNum(interpLoops); PrintNewLine();
  #endif

  return ! interp->run;
}
