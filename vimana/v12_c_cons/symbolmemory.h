/*
File: symbolmemory.h
Author: Mikael Kindborg (mikael@kindborg.com)

Compact way to allocate immutable symbols within a block 
of memory (no need for malloc of individual symbols).

Memory layout:

Symbols are allocated in sequence.
0 char is the string terminator.

Example with three symbol entries (0 is char zero '\0'):

  One0Two0Tree0
*/

struct __VSymbolMemory
{
  char*  start;    // Start of string memory
  char*  nextFree; // Points to next free block in string memory
  char*  pos;      // Current position to write to
  int    size;     // Max number of chars that can be stored in string memory
}
VSymbolMemory;

// Global symbol memory
static VSymbolMemory* GlobalSymbolMemory = NULL;  

void GlobalSymbolMemoryInit(void* mem, int size)
{
  GlobalSymbolMemory = mem;
  SymbolMemInit(mem, size);
}

VSymbolMemory* GlobalSymbolMemoryGet()
{
  return GlobalSymbolMemory;
}

// Return number of bytes needed to hold header struct plus array
int SymbolMemGetByteSize(int size)
{
  return sizeof(VSymbolMemory) + (sizeof(char) * size);
}

void SymbolMemInit(VSymbolMemory* mem, int size)
{
  mem->start = (char**) PtrOffset(mem, sizeof(VSymbolMemory));
  mem->nextFree = mem->start;
  mem->pos = mem->nextFree;
  mem->size = size;
}

// Return pointer to next free block
char* SymbolMemGetNextFree(VSymbolMemory* mem)
{
  return mem->nextFree;
}

// Finish writing to the current block
void SymbolMemWriteFinish(VSymbolMemory* mem)
{
  ++ mem->pos;
  mem->nextFree = mem->pos;
}

// Reset the write position to beginning of free block
void SymbolMemResetPos(VSymbolMemory* mem)
{
  mem->pos = mem->nextFree;
}

// Write a character
void SymbolMemWriteChar(VSymbolMemory* mem, char c)
{
  if (mem->pos + 2 >= mem->start + size)
  {
    GURU_MEDITATION(SYMBOL_MEMORY_OUT_OF_SPACE);
  }

  *(mem->pos) = c;
  ++ mem->pos;
  *(mem->pos) = 0;
}
