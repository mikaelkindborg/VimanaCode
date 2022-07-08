/*
File: machine.h
Author: Mikael Kindborg (mikael@kindborg.com)

Functions for creating the machine that runs VimanaCode.
*/

// -------------------------------------------------------------
// System memory
// -------------------------------------------------------------

static VByte* GlobalMemory;
static VByte* GlobalInterpMemoryStart;
static size_t GlobalMemoryByteSize;

void MachineAllocate(size_t numBytes)
{
  GlobalMemory = SysAlloc(numBytes);
  GlobalMemoryFirstFree = GlobalMemory;
  GlobalMemoryByteSize = numBytes;
}

void MachineDeallocate()
{
  SysFree(GlobalMemory);
}

// Single global interpreter instance (note that it would be  
// possible to have multiple interperter instances)
static VInterp* GlobalMachineInterp;

VInterp* MachineInterp()
{
  return GlobalMachineInterp;
}

// -------------------------------------------------------------
// Create the machine
// -------------------------------------------------------------

/*
How to use:

#define MACHINE_BYTE_SIZE (1024 * 16)  // 16KB
#define NUM_SYMBOLS 100
#define DATA_STACK_SIZE 100
#define CALL_STACK_SIZE 100
// Item memory takes the rest of free memory

MachineAllocate(MACHINE_BYTE_SIZE);
MachineAddCorePrimFuns();
AddExtraPrimFuns();
MachineCreateSymbolTable(NUM_SYMBOLS);
MachineCreateInterp(DATA_STACK_SIZE, CALL_STACK_SIZE);

MachineAllocate(numBytes);
MachineAddCorePrimFuns();
AddExtraPrimFuns();
MachineCreateSymbolTable(numSymbols);
MachineCreateInterp(dataStackSize, callStackSize);
*/

void MachineAddCorePrimFuns()
{
  PrimFunTableInit(GlobalMemory);
  AddCorePrimFuns();
}

void MachineCreateSymbolTable(int numSymbols)
{
  VByte* symbolTableStart = GlobalMemory + PrimFunTableByteSize();
  SymbolTableInit(symbolTableStart, numSymbols);

  VByte* symbolMemoryStart = symbolTableStart + SymbolTableByteSize();
  int numChars = numSymbols * 10; // Estimate 10 chars per symbol
  SymbolMemoryInit(symbolMemoryStart, numChars);

  GlobalInterpMemoryStart = symbolMemoryStart + SymbolMemoryByteSize();
}

VInterp* MachineCreateInterp(int dataStackSize, int callStackSize)
{
  int numSymbols = SymbolTableMaxSize();
  int interpBaseByteSize = InterpByteSizeExcludingItemMemory(numSymbols, dataStackSize, callStackSize);
  int itemMemoryByteSize =  interpBaseByteSize GlobalMemoryByteSize


  GlobalMachineInterp = InterpInit(GlobalInterpMemoryStart, dataStackSize, callStackSize, itemMemorySize)

  VByte* symbolTableStart = MachineMemory() + PrimFunTableByteSize();
  SymbolTableInit(symbolTableStart, numSymbols);

  VByte* symbolMemoryStart = symbolTableStart + SymbolTableByteSize(numSymbols);
  int numChars = numSymbols * 10; // Estimate 10 chars per symbol
  SymbolMemoryInit(symbolMemoryStart, numChars);

  GlobalMemoryFirstFree = symbolMemoryStart + SymbolMemoryByteSize(numChars);
}

void MachineParse()
{
}

void MachineEval()
{
}