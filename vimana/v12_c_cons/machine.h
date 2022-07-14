/*
File: machine.h
Author: Mikael Kindborg (mikael@kindborg.com)

High-level functions for creating the machine that runs VimanaCode.

Creates a single interpreter (multiple interpreters would also be possible).

The primfun table and the symbol table are global and shared by interpreters.

Memory is preallocated in a single block. This is by design to support Arduino 
and other small computers with memory of known size.

How to use:

  #define MACHINE_BYTE_SIZE (128 * 1024)  // Size in bytes
  #define SYMBOL_TABLE_SIZE 100           // Number of symbols in symbol table
  #define DATA_STACK_SIZE 100             // Number of items on the data stack
  #define CALL_STACK_SIZE 100             // Number of stack frames
  #define LIST_MEMORY_SIZE 1000           // Number of items in garbage collected memory

  MachineAllocate(MACHINE_BYTE_SIZE);
  MachineAddCorePrimFuns();
  //AddCustomPrimFuns();
  MachineCreate(
    SYMBOL_TABLE_SIZE, 
    DATA_STACK_SIZE, 
    CALL_STACK_SIZE, 
    LIST_MEMORY_SIZE);
  MachineEval("(Hi World) print");
*/

// -------------------------------------------------------------
// Globals
// -------------------------------------------------------------

// System memory
static VByte*  GlobalMemory;
static VByte*  GlobalMemoryEnd;
static int     GlobalMemoryByteSize;

// Single global interpreter instance (note that it would be  
// possible to have multiple interperter instances)
static VByte*  GlobalInterp;
static int     GlobalInterpByteSize;


// -------------------------------------------------------------
// System memory
// -------------------------------------------------------------

void MachineAllocate(size_t numBytes)
{
  GlobalMemory = SysAlloc(numBytes);
  GlobalMemoryByteSize = numBytes;
}

void MachineDeallocate()
{
  SysFree(GlobalMemory);
}

// -------------------------------------------------------------
// Interpreter instance
// -------------------------------------------------------------

VInterp* MachineInterp()
{
  return (VInterp*) GlobalInterp;
}

// -------------------------------------------------------------
// Create the machine
// -------------------------------------------------------------

void MachineAddCorePrimFuns()
{
  PrimFunTableInit(GlobalMemory);
  AddCorePrimFuns();
}

void MachineCreateSymbolTable(int symbolTableSize)
{
  VByte* symbolTableStart = GlobalMemory + PrimFunTableByteSize();
  SymbolTableInit(symbolTableStart, symbolTableSize);

  VByte* symbolMemoryStart = symbolTableStart + SymbolTableByteSize();
  int numChars = symbolTableSize * 10; // Estimate 10 chars per symbol
  SymbolMemInitGlobal(symbolMemoryStart, numChars);

  GlobalInterp = symbolMemoryStart + SymbolMemByteSizeGlobal();
}

void MachineCreateInterp(int dataStackSize, int callStackSize, int listMemorySize)
{
  int numSymbols = SymbolTableMaxSize();

  GlobalInterpByteSize = InterpByteSize(numSymbols, dataStackSize, callStackSize, listMemorySize);

  GlobalMemoryEnd = GlobalInterp + GlobalInterpByteSize;

  // Check that interpreter will fit in allocated memory
  if (GlobalMemoryEnd > GlobalMemory + GlobalMemoryByteSize)
  {
    GURU_MEDITATION(MACHINE_OUT_OF_MEMORY);
  }

  InterpInit(
    (VInterp*) GlobalInterp, 
    numSymbols, dataStackSize, callStackSize, listMemorySize);
}

void MachineCreate(
  int symbolTableSize, int dataStackSize, 
  int callStackSize,   int listMemorySize)
{
  MachineCreateSymbolTable(symbolTableSize);
  MachineCreateInterp(dataStackSize, callStackSize, listMemorySize);
}

void MachineFree()
{
  InterpFree(MachineInterp());
  MachineDeallocate();
}

void MachinePrintMemoryUse()
{
  int memoryUsed = GlobalMemoryEnd - GlobalMemory;

  printf("Vimana Machine Memory Use\n");
  printf("-------------------------\n");
  printf("Allocated:      %i\n", GlobalMemoryByteSize);
  printf("Used:           %i\n", memoryUsed);
  printf("Primfun table:  %i\n", PrimFunTableByteSize());
  printf("Symbol table:   %i\n", SymbolTableByteSize());
  printf("Symbol memory:  %i\n", SymbolMemByteSizeGlobal());
  printf("Interpreter:    %i\n", GlobalInterpByteSize);
}

void MachineEval(char* code)
{
  InterpEval(MachineInterp(), 
    InterpParse(MachineInterp(), code));
}
